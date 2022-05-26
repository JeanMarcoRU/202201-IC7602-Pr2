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
        printf("se leyo %d\n", bytes_read);

        if (buffer[2] & 0x01 && (buffer[2] & 0x1e) == 0)
        {
            printf("encontre lo que busco\n");
        }

        /*
        convierte a base64 el buffer (binario)
        el encode en base64 se guarda en 'enc'
        Funte del codigo: https://nachtimwald.com/2017/11/18/base64-encode-and-decode-in-c/
        */
        char *enc;
        size_t out_len;

        enc = b64_encode(buffer, bytes_read);
        printf("encoded: '%s'\n", enc);

        char *datapost = malloc(MAXSIZE);
        sprintf(datapost, "http://localhost:443/api/dns_resolver?data=%s", enc);

        CURL *curl;
        CURLcode response;

        curl_global_init(CURL_GLOBAL_ALL);

        curl = curl_easy_init();
        if (curl)
        {
            curl_easy_setopt(curl, CURLOPT_URL, datapost);

            response = curl_easy_perform(curl);

            if (response != CURLE_OK)
            {
                fprintf(stderr, "Request failed: %s\n", curl_easy_strerror(response));
            }
            /*else
            {
                printf(response);
            }*/
            curl_easy_cleanup(curl);
        }
        curl_global_cleanup();

        /*
        Convierte de base64 a binario
        el decode en se guarda en 'out'
        */
        /* +1 for the NULL terminator. */
        out_len = b64_decoded_size(enc);
        unsigned char out[out_len];
        // out = malloc(out_len);

        if (!b64_decode(enc, out, out_len))
        {
            printf("Decode Failure\n");
            return 1;
        }

        // lo guarda en un txt para comprobar que no hay perdida de datos
        f2 = fopen("logtest.txt", "wb");
        fwrite(out, 1, out_len, f2);

        // buffer[bytes_read] = '\0';
        /*for (int i = 0; i < bytes_read; i+=2)
        {
            char l[9], h[9];
            sprintf(l, BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(buffer[i]));
            sprintf(h, BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(buffer[i+1]));
            //printf("+----------------+\n|%c%c|\n", buffer[i], buffer[i+1]);
            //printf("+----------------+\n|%s%s|\n", l, h);
        }*/

        /*char *end_msj = malloc(MAXSIZE);
        sprintf(end_msj, "Server: %s\n", buffer);

        // envia el mensaje
        if (sendto(sock, end_msj, strlen(end_msj), 0, (struct sockaddr *)&client_addr, addr_len) == -1)
        {
            printf("Error: sendto()");
        }*/

        fflush(stdout);
        fclose(f1);
        fclose(f2);
    }
    return 0;
}