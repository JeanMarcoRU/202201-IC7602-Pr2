#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <curl/curl.h>

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)       \
    (byte & 0x80 ? '1' : '0'),     \
        (byte & 0x40 ? '1' : '0'), \
        (byte & 0x20 ? '1' : '0'), \
        (byte & 0x10 ? '1' : '0'), \
        (byte & 0x08 ? '1' : '0'), \
        (byte & 0x04 ? '1' : '0'), \
        (byte & 0x02 ? '1' : '0'), \
        (byte & 0x01 ? '1' : '0')

#define PORT 53
#define MAXSIZE 2048

const char b64chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

const int b64invs[] = {62, -1, -1, -1, 63, 52, 53, 54, 55, 56, 57, 58,
                       59, 60, 61, -1, -1, -1, -1, -1, -1, -1, 0, 1, 2, 3, 4, 5,
                       6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
                       21, 22, 23, 24, 25, -1, -1, -1, -1, -1, -1, 26, 27, 28,
                       29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42,
                       43, 44, 45, 46, 47, 48, 49, 50, 51};

static size_t writeCallback(void *contents, size_t size, size_t nitems, FILE *file)
{
    return fwrite(contents, size, nitems, file);
}

// Since a little math is involved to determine the encoded size we’ll use this simple function to help us out.
size_t b64_encoded_size(size_t inlen)
{
    size_t ret;

    ret = inlen;
    if (inlen % 3 != 0)
        ret += 3 - (inlen % 3);
    ret /= 3;
    ret *= 4;

    return ret;
}

/*
Now we need to move the mapping table.
Unlike decoding this is an array which we’ll reference by index as we encode.
This table is used to turn 3 byte sequences into characters.
*/
char *b64_encode(const unsigned char *in, size_t len)
{
    char *out;
    size_t elen;
    size_t i;
    size_t j;
    size_t v;

    if (in == NULL || len == 0)
        return NULL;

    elen = b64_encoded_size(len);
    out = malloc(elen + 1);
    out[elen] = '\0';

    for (i = 0, j = 0; i < len; i += 3, j += 4)
    {
        v = in[i];
        v = i + 1 < len ? v << 8 | in[i + 1] : v << 8;
        v = i + 2 < len ? v << 8 | in[i + 2] : v << 8;

        out[j] = b64chars[(v >> 18) & 0x3F];
        out[j + 1] = b64chars[(v >> 12) & 0x3F];
        if (i + 1 < len)
        {
            out[j + 2] = b64chars[(v >> 6) & 0x3F];
        }
        else
        {
            out[j + 2] = '=';
        }
        if (i + 2 < len)
        {
            out[j + 3] = b64chars[v & 0x3F];
        }
        else
        {
            out[j + 3] = '=';
        }
    }

    return out;
}

/*
Much like encoding, we need to do some math to determine binary data size
of some Base64 encoded data we might have.
*/
size_t b64_decoded_size(const char *in)
{
    size_t len;
    size_t ret;
    size_t i;

    if (in == NULL)
        return 0;

    len = strlen(in);
    ret = len / 4 * 3;

    for (i = len; i-- > 0;)
    {
        if (in[i] == '=')
        {
            ret--;
        }
        else
        {
            break;
        }
    }

    return ret;
}

/*
I don’t know about you but I hate magic numbers someone on the internet says to use.
I’m fine with using them as long as I can verify they’re correct.
*/
void b64_generate_decode_table()
{
    int inv[80];
    size_t i;

    memset(inv, -1, sizeof(inv));
    for (i = 0; i < sizeof(b64chars) - 1; i++)
    {
        inv[b64chars[i] - 43] = i;
    }
}

/*
We need to subtract 43 to shift + to be the 0 index. Once we run this function we’ll end up with the above
encoding table. It’s a better idea to use the precomputed table than generate it anew for every decode.
*/
int b64_isvalidchar(char c)
{
    if (c >= '0' && c <= '9')
        return 1;
    if (c >= 'A' && c <= 'Z')
        return 1;
    if (c >= 'a' && c <= 'z')
        return 1;
    if (c == '+' || c == '/' || c == '=')
        return 1;
    return 0;
}

/*
We’ll need a verification function to check if characters are are valid for the base64 character set. This should be
expanded to handle newlines, deal with line length requirements, ignore whitespace if necessary, and verify there
are two or less = , and = is only present at the end. Verification could also happen in the main decode function
instead of being split out. I find it easier to split it out otherwise the decode function becomes more difficult
to understand.
*/
int b64_decode(const char *in, unsigned char *out, size_t outlen)
{
    size_t len;
    size_t i;
    size_t j;
    int v;

    if (in == NULL || out == NULL)
        return 0;

    len = strlen(in);
    if (outlen < b64_decoded_size(in) || len % 4 != 0)
        return 0;

    for (i = 0; i < len; i++)
    {
        if (!b64_isvalidchar(in[i]))
        {
            return 0;
        }
    }

    for (i = 0, j = 0; i < len; i += 4, j += 3)
    {
        v = b64invs[in[i] - 43];
        v = (v << 6) | b64invs[in[i + 1] - 43];
        v = in[i + 2] == '=' ? v << 6 : (v << 6) | b64invs[in[i + 2] - 43];
        v = in[i + 3] == '=' ? v << 6 : (v << 6) | b64invs[in[i + 3] - 43];

        out[j] = (v >> 16) & 0xFF;
        if (in[i + 2] != '=')
            out[j + 1] = (v >> 8) & 0xFF;
        if (in[i + 3] != '=')
            out[j + 2] = v & 0xFF;
    }

    return 1;
}

void leer_registro(int out[2])
{
    FILE *fptr;
    fptr = fopen("registro.json", "r");

    char str_ip[17] = {'\0'};
    int ttl;

    size_t len = 0;
    char *buffer;
    ssize_t read;
    read = getline(&buffer, &len, fptr);          // Esta es la línea que tiene solo el {
    read = getline(&buffer, &len, fptr);          // Esta línea tiene el nombre de dominio
    fscanf(fptr, "    \"TTL\": \"%d\",\n", &ttl); //     "TTL": "[0-9]+",
    fscanf(fptr, "    \"IP\": \"%s\"\n", str_ip); //    "IP": "10.0.5.2"
    fclose(fptr);

    str_ip[strlen(str_ip) - 1] = '\0';
    // DEPURACIÓN
    // printf("ttl: %d, ip: %s\n", ttl, str_ip);

    out[0] = ttl;
    out[1] = inet_addr(str_ip);
}

void generar_paquete(unsigned char *consulta, int qs, int ttl, int ip)
{
    consulta[2] = 0x81;
    consulta[3] = 0x80;
    consulta[7] = 1;
    consulta[qs] = 0xc0;
    consulta[qs + 1] = 0x0c;
    consulta[qs + 2] = 0x00;
    consulta[qs + 3] = 0x01;
    consulta[qs + 4] = 0x00;
    consulta[qs + 5] = 0x01;
    consulta[qs + 6] = 0x00;
    consulta[qs + 7] = 0x00;
    consulta[qs + 8] = (ttl & 0xff00) >> 8;
    consulta[qs + 9] = (ttl & 0xff);
    consulta[qs + 10] = 0x00;
    consulta[qs + 11] = 0x04;
    consulta[qs + 12] = (ip & 0xff);
    consulta[qs + 13] = (ip & 0xff00) >> 8;
    consulta[qs + 14] = (ip & 0xff0000) >> 16;
    consulta[qs + 15] = (ip & 0xff000000) >> 24;
}

int main()
{
    int sock;
    int addr_len, bytes_read;
    unsigned char buffer[MAXSIZE];
    struct sockaddr_in server_addr, client_addr;

    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        perror("Socket");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    bzero(&(server_addr.sin_zero), 8);

    if (bind(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1)
    {
        perror("Bind");
        exit(1);
    }

    addr_len = sizeof(struct sockaddr);

    printf("UDPServer Waiting for client on port 53\n");
    fflush(stdout);
    FILE *f1;
    FILE *f2;

    while (1)
    {

        // recibe el mensaje
        bytes_read = recvfrom(sock, buffer, 1024, 0, (struct sockaddr *)&client_addr, &addr_len);
        f1 = fopen("log.txt", "wb");
        fwrite(buffer, 1, bytes_read, f1);
        fclose(f1);

        char *enc;
        size_t out_len;

        if (buffer[2] & 0x01 && (buffer[2] & 0x1e) == 0)
        {
            printf("\nLlegó un paquete query estándar.\n");
            // para consultar elasticsearch se requiere el nombre de dominio del paquete
            char hostname[MAXSIZE] = {'\0'};

            int i = 12;
            int k = 0;
            while (buffer[i] != 0)
            {
                for (int j = i + 1; j <= i + buffer[i]; j++)
                    hostname[k++] = buffer[j];
                i += buffer[i] + 1;
                if (buffer[i] != 0)
                    hostname[k++] = '.';
            }

            printf("Domain name: %s\n", hostname);

            char *dataget = malloc(MAXSIZE);
            sprintf(dataget, "http://elasticsearch:9200/zones/_doc/_search?q=hostname:%s", hostname);

            FILE *file = fopen("registro.json", "w");
            if (!file)
            {
                fprintf(stderr, "Could not open output file.\n");
                return 1;
            }

            CURL *curl;
            CURLcode res;
            curl_global_init(CURL_GLOBAL_ALL);
            curl = curl_easy_init();
            if (curl)
            {
                curl_easy_setopt(curl, CURLOPT_URL, dataget);
                curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
                curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
                curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)file);
                res = curl_easy_perform(curl);
                if (res != CURLE_OK)
                    fprintf(stderr, "curl_easy_perform() failed: %s\n",
                            curl_easy_strerror(res));
                curl_easy_cleanup(curl);
            }
            curl_global_cleanup();
            free(dataget);
            fclose(file);
            // if resultado ==
            f1 = fopen("registro.json", "r");

            char str_ip[17] = {'\0'};
            int ttl;

            size_t len = 0;
            char *texto;
            ssize_t read;

            read = getline(&texto, &len, f1);
            if (texto[2] == 't' && texto[strcspn(texto, "[]") + 1] != ']')
            { // Esto quiere decir que hay matches
                fseek(f1, strcspn(texto, "T"), SEEK_SET);
                fscanf(f1, "TTL\": \"%d\",\"IP\": \"%s,", &ttl, str_ip); //     "TTL": "[0-9]+",
                read = getline(&texto, &len, f1);
                
                if (str_ip[strcspn(str_ip, ",\"")] == ','){
                    char otrosIPs[300];
                    for (int z = 0; z < strlen(texto) - 6; z++)
                        otrosIPs[z] = texto[z + 1];
                    short s = strcspn(otrosIPs, "\"");
                    otrosIPs[s++] = ',';
                    otrosIPs[s++] = ' ';
                    for (int z = 0; z < strlen(str_ip) - 1; z++)
                        otrosIPs[s + z] = str_ip[z];
                    //otrosIPs[strlen(otrosIPs)-strlen(str_ip)] = '\0';
                    printf("Para actualizar la base: %s\n", otrosIPs);
                } else {
                    str_ip[strcspn(str_ip, ",\"")] = '\0';
                }
                generar_paquete(buffer, bytes_read, ttl, inet_addr(str_ip));
                
                f2 = fopen("log2.txt", "wb");
                fwrite(buffer, 1, bytes_read + 16, f2);
                fclose(f2);

                printf("Se resolvió sin ir al api.\n");

                sendto(sock, buffer, bytes_read + 16, 0, (struct sockaddr *)&client_addr, addr_len);
                

                fflush(stdout);
                continue;
            }
            fclose(f1);
        }

        // el encode en base64 se guarda en 'enc'
        // Fuente del codigo: https://nachtimwald.com/2017/11/18/base64-encode-and-decode-in-c/

        enc = b64_encode(buffer, bytes_read);
        printf("encoded: '%s'\n", enc);

        // char *urllink = "http://localhost:443/api/dns_resolver";
        char *datapost = malloc(MAXSIZE);
        sprintf(datapost, "http://restapi:443/api/dns_resolver?data=%s", enc);

        FILE *file = fopen("received.txt", "wb");
        if (!file)
        {
            fprintf(stderr, "Could not open output file.\n");
            return 1;
        }

        CURL *curl;
        CURLcode res;
        curl_global_init(CURL_GLOBAL_ALL);
        curl = curl_easy_init();
        if (curl)
        {
            curl_easy_setopt(curl, CURLOPT_URL, datapost);
            curl_easy_setopt(curl, CURLOPT_POST, 1L);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)file);
            res = curl_easy_perform(curl);
            if (res != CURLE_OK)
                fprintf(stderr, "curl_easy_perform() failed: %s\n",
                        curl_easy_strerror(res));
            curl_easy_cleanup(curl);
        }
        curl_global_cleanup();
        free(datapost);
        fclose(file);

        FILE *archivo = fopen("received.txt", "r"); // Modo lectura
        char todecode[MAXSIZE];                     // Aquí vamos a ir almacenando cada línea
        fgets(todecode, MAXSIZE, archivo);
        fclose(archivo);

        out_len = b64_decoded_size(todecode);
        unsigned char out[out_len];
        // out = malloc(out_len);

        if (!b64_decode(todecode, out, out_len))
        {
            printf("Decode Failure\n");
            return 1;
        }

        // lo guarda en un txt para comprobar que no hay perdida de datos
        f2 = fopen("nslookup.txt", "wb");
        fwrite(out, 1, out_len, f2);
        fclose(f2);
        if (sendto(sock, out, out_len, 0, (struct sockaddr *)&client_addr, addr_len) == -1)
        {
            printf("Error: sendto()");
        }

        fflush(stdout);
    }
    return 0;
}