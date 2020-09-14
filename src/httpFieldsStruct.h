/**
 * Http Request fields.
 * The httpFields struct is used to parse the http
 * request and is filled with appropriate data.
*/
struct httpFields {
    char method[5];
    char file[256];
    char version[10];
};
typedef struct httpFields *httpFieldsPtr;
