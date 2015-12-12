#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <cose.h>
#include <sys/stat.h>
#include <cn-cbor/cn-cbor.h>
#include <io.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

typedef struct _FOO {
    char *      fieldName;
    int         type;
	int			value;
    struct _FOO * children;
	int         count;
	int			group;
} FOO;

FOO Recipients;
extern FOO Signer[];

FOO AlgorithmMap[32] = {
	{ "ES512", CN_CBOR_INT, -9 },
	{ "ES384", CN_CBOR_INT, -8 },
	{"ES256", CN_CBOR_INT, -7},
	{"direct", CN_CBOR_INT, -6},
	{"A256KW", CN_CBOR_INT, -5},
	{"A192KW", CN_CBOR_INT, -4},
	{"A128KW", CN_CBOR_INT, -3},
	{"AES-GCM 128", CN_CBOR_UINT, 1},
	{"AES-GCM 192", CN_CBOR_UINT, 2},
	{"AES-GCM 256", CN_CBOR_UINT, 3},
	{"HMAC 256//256", CN_CBOR_UINT, 4},
	{"HMAC 384//384", CN_CBOR_UINT, 5},
	{"HMAC 512//512", CN_CBOR_UINT, 6},
	{"AES-CCM-16-64-128", CN_CBOR_UINT, 10},
	{"AES-CCM-16-64-256", CN_CBOR_UINT, 11},
	{"AES-CCM-16-128-128", CN_CBOR_UINT, 12},
	{"AES-CCM-16-128-256", CN_CBOR_UINT, 13},
	{"ChaCha20//Poly1305", CN_CBOR_UINT, 24},
	{"AES-CCM-64-64-128", CN_CBOR_UINT, 30},
	{"AES-CCM-64-64-256", CN_CBOR_UINT, 31},
	{"AES-CCM-64-128-128", CN_CBOR_UINT, 32},
	{"AES-CCM-64-128-256", CN_CBOR_UINT, 33},
	{"ECDH-ES + HKDF-256", CN_CBOR_UINT, 50},
	{"ECDH-ES + HKDF-512", CN_CBOR_UINT, 51},
	{"ECDH-SS + HKDF-256", CN_CBOR_UINT, 52},
	{"ECDH-SS + HKDF-512", CN_CBOR_UINT, 53},
	{"ECHD-ES+A128KW", CN_CBOR_UINT, 54},
	{ "ECHD-ES+A192KW", CN_CBOR_UINT, 55 },
	{ "ECHD-ES+A256KW", CN_CBOR_UINT, 56 },
	{ "ECHD-SS+A128KW", CN_CBOR_UINT, 57 },
	{ "ECHD-SS+A192KW", CN_CBOR_UINT, 58 },
	{ "ECHD-SS+A256KW", CN_CBOR_UINT, 59 },
};

FOO KeyMap[9] = {
	{"kty", CN_CBOR_UINT, 1 },
	{"kid", CN_CBOR_UINT, 2 },
	{"alg", CN_CBOR_UINT, 3, AlgorithmMap, _countof(AlgorithmMap) },
	{"key_ops", CN_CBOR_UINT, 4},
	{"crv", CN_CBOR_INT, -1, NULL, 0, 2},
	{"x", CN_CBOR_INT, -2, NULL, 0, 2},
	{"y", CN_CBOR_INT, -3, NULL, 0, 2},
	{"d", CN_CBOR_INT, -4, NULL, 0, 2},
	{"k", CN_CBOR_INT, -1, NULL, 0, 4}
};

FOO Key = {
	NULL, CN_CBOR_MAP, 0, KeyMap, _countof(KeyMap)
};

FOO KeySet = {
	NULL, CN_CBOR_ARRAY, 0, &Key, 1
};

FOO HeaderMap[26] = {
	{ "alg", CN_CBOR_UINT, 1, AlgorithmMap, _countof(AlgorithmMap) },
	{ "crit", CN_CBOR_UINT, 2, NULL },
	{ "content type", CN_CBOR_UINT, 3, NULL},
	{ "kid", CN_CBOR_UINT, 4, NULL},
	{ "iv", CN_CBOR_UINT, 5, NULL },
	{ "partial iv", CN_CBOR_UINT, 6, NULL },
	{ "countersign", CN_CBOR_UINT, 7, Signer, 5 },
	{ "ephemeral", CN_CBOR_INT, -1, KeyMap, _countof(KeyMap), 50},
{"salt", CN_CBOR_INT, -20, NULL, 0, 50 },
{ "U identity", CN_CBOR_INT, -21, NULL, 0, 50 },
{ "U nonce", CN_CBOR_INT, -22, NULL, 0, 50 },
{ "U other", CN_CBOR_INT, -23, NULL, 0, 50 },
{ "V identity", CN_CBOR_INT, -24, NULL, 0, 50 },
{ "V nonce", CN_CBOR_INT, -25, NULL, 0, 50 },
{ "V other", CN_CBOR_INT, -26, NULL, 0, 50 },
{ "static key", CN_CBOR_INT, -2, NULL, 0, 50},
{ "static kid", CN_CBOR_INT, -3, NULL, 0, 50},
{"salt", CN_CBOR_INT, -20, NULL, 0, 52 },
{ "U identity", CN_CBOR_INT, -21, NULL, 0, 52 },
{ "U nonce", CN_CBOR_INT, -22, NULL, 0, 52 },
{ "U other", CN_CBOR_INT, -23, NULL, 0, 52 },
{ "V identity", CN_CBOR_INT, -24, NULL, 0, 52 },
{ "V nonce", CN_CBOR_INT, -25, NULL, 0, 52 },
{ "V other", CN_CBOR_INT, -26, NULL, 0, 52 },
{ "static key", CN_CBOR_INT, -2, NULL, 0, 52 },
{ "static kid", CN_CBOR_INT, -3, NULL, 0, 52 }
};

FOO RecurseHeaderMap = {
	NULL, CN_CBOR_MAP, 0, HeaderMap, _countof(HeaderMap)
};

FOO EncryptedBody[4] = {
    { "protected", CN_CBOR_BYTES, 0, &RecurseHeaderMap, 1 },
    { "unprotected", CN_CBOR_MAP, 0, HeaderMap, _countof(HeaderMap) },
    { "ciphertext", CN_CBOR_BYTES, 0, NULL },
    { "recipients", CN_CBOR_ARRAY, 0, &Recipients, 1 }
};

FOO Recipients = {
    NULL, CN_CBOR_ARRAY, 0, EncryptedBody, _countof(EncryptedBody)
};

FOO MacBody[5] = {
	{ "protected", CN_CBOR_BYTES, 0, &RecurseHeaderMap, 1 },
	{ "unprotected", CN_CBOR_MAP, 0, HeaderMap, _countof(HeaderMap) },
	{ "payload", CN_CBOR_BYTES, 0, NULL },
	{ "tag", CN_CBOR_BYTES, 0, NULL },
	{ "recipients", CN_CBOR_ARRAY, 0, &Recipients, 1 }
};

FOO Signer[5] = {
	{ "protected", CN_CBOR_BYTES, 0, &RecurseHeaderMap, 1 },
	{ "unprotected", CN_CBOR_MAP, 0, HeaderMap, _countof(HeaderMap) },
	{ "signature", CN_CBOR_BYTES, 0, NULL },
};

FOO Signers = {
	NULL, CN_CBOR_ARRAY, 0, Signer, _countof(Signer)
};

FOO SignBody[5] = {
	{ "protected", CN_CBOR_BYTES, 0, &RecurseHeaderMap, 1 },
	{ "unprotected", CN_CBOR_MAP, 0, HeaderMap, _countof(HeaderMap) },
	{ "payload", CN_CBOR_BYTES, 0, NULL },
	{ "signatures", CN_CBOR_ARRAY, 0, &Signers, 1 }
};

FOO EnvelopedMessage = {
	NULL, CN_CBOR_ARRAY, 0, EncryptedBody, _countof(EncryptedBody)
};
FOO SignedMessage = {
	NULL, CN_CBOR_ARRAY, 0, SignBody, _countof(SignBody)
};
FOO MacMessage = {
	NULL, CN_CBOR_ARRAY, 0, MacBody, _countof(MacBody)
};

FOO EncryptedMessage = {
    NULL, CN_CBOR_ARRAY, 0, EncryptedBody, _countof(EncryptedBody)-1
};

FOO EncryptedMessageWithTag = {
    NULL, CN_CBOR_TAG, 997, &EncryptedMessage, 1
};

FOO EnvelopedMessageWithTag = {
	NULL, CN_CBOR_TAG, 998, &EnvelopedMessage, 1
};

FOO SignedMessageWithTag = {
	NULL, CN_CBOR_TAG, 999, &SignedMessage, 1
};

FOO MacMessageWithTag = {
	NULL, CN_CBOR_TAG, 996, &MacMessage, 1
};

int WrapLineAt = 0;
char OutputBuffer[4096];

void WrapPrintF(FILE * fp, char * format, ...)
{
	va_list args;
	char	buffer[4000];
	char *  iRet;

	va_start(args, format);
	vsprintf(buffer, format, args);
	if (WrapLineAt == 0) {
		fprintf(fp, buffer);
		return;
	}

	strcat_s(OutputBuffer, sizeof(OutputBuffer), buffer);

	while (iRet = strchr(OutputBuffer, '\n')) {
		char * t = OutputBuffer;
		*iRet = 0;

		while (strlen(t) > WrapLineAt) {
			char x = t[WrapLineAt];
			t[WrapLineAt] = 0;
			fprintf(fp, t);
			fprintf(fp, "\n");
			t[WrapLineAt] = x;
			t += WrapLineAt;
		}

		fprintf(fp, t);
		fprintf(fp, "\n");
		strcpy(OutputBuffer, iRet + 1);
	}
}


void Indent(FILE * fp, int depth)
{
	int i;

	for (i = 0; i < depth; i++) WrapPrintF(fp, "  ");
}


void PrintUsage()
{
    fprintf(stderr, "dumper [<filein> [<fileout>]]\n");
    exit(1);
}

void DumpBytes(FILE * fp, const cn_cbor* cbor, int depth)
{
	int i;
	int fText = true;

	for (i = 0; i < cbor->length; i++) {
		if ((cbor->v.bytes[i] < 32) || (cbor->v.bytes[i] > 126)) fText = false;
	}

	if (fText && (cbor->length > 0)) {
		WrapPrintF(fp, "'");
		for (i = 0; i < cbor->length; i++) {
			WrapPrintF(fp, "%c", cbor->v.bytes[i]);
		}
		WrapPrintF(fp, "'");
	}
	else {
		WrapPrintF(fp, "h'");
		for (i = 0; i < cbor->length; i++) {
			WrapPrintF(fp, "%02x", cbor->v.bytes[i]);
		}
		WrapPrintF(fp, "'");
	}
}

void DumpTree(const cn_cbor * cbor, FILE * out, FOO *pFOO, int depth, int fField, int fValue, int fInComment)
{
	int i;
	int i2;
	const cn_cbor * cbor2;
	const FOO * pFoo2;
	int group;

	if (pFOO != NULL) {
		switch (pFOO->type) {
		case CN_CBOR_TAG:
			if (cbor->type != CN_CBOR_TAG) pFOO = NULL;
			break;
		default:
			break;
		}
	}

	if (fField && (pFOO != NULL) && (pFOO->fieldName != NULL)) {
		if (fInComment) WrapPrintF(out, "\\ %s \\ ", pFOO -> fieldName);
		else WrapPrintF(out, "/ %s / ", pFOO->fieldName);
	}

	switch (cbor->type) {
	case CN_CBOR_TAG:
		WrapPrintF(out, "%u(\n", cbor->v.uint);
		Indent(out, depth + 1);
		DumpTree(cbor->last_child, out, pFOO != NULL ? pFOO->children : NULL, depth+1, true, true, fInComment);
		WrapPrintF(out, "\n");
		Indent(out, depth);
		WrapPrintF(out, ")");
		break;

	case CN_CBOR_ARRAY:
		WrapPrintF(out, "[");
		cbor2 = cbor->first_child;
		for (i = 0; i < cbor->length; i++, cbor2 = cbor2->next) {
			if (i != 0) WrapPrintF(out, ", ");
			if (pFOO == NULL) pFoo2 = NULL;
			else if (pFOO->count == 1) pFoo2 = pFOO->children;
			else if (i >= pFOO->count) pFoo2 = NULL;
			else pFoo2 = &pFOO->children[i];

			WrapPrintF(out, "\n");
			Indent(out, depth + 1);
			DumpTree(cbor2, out, pFoo2, depth + 1, true, true, fInComment);
		}
		if (i > 0) {
			WrapPrintF(out, "\n");
			Indent(out, depth);
		}
				WrapPrintF(out, "]");
		break;

	case CN_CBOR_MAP:
		WrapPrintF(out, "{");
		cbor2 = cbor->first_child;
		//  Determine the correct group - always assume it is at element UINT=1
		group = 0;
		for (i = 0; i < cbor->length; i+=2, cbor2 = cbor2->next->next) {
			if ((cbor2->type == CN_CBOR_UINT) && (cbor2->v.uint == 1)) {
				group = cbor2->next->v.uint;
				break;
			}
		}

		cbor2 = cbor->first_child;
		//  Dump each element
		for (i = 0; i < cbor->length; i+=2, cbor2 = cbor2->next) {
			pFoo2 = NULL;
			if (pFOO != NULL) {
				//  Locate the right entry in foo
				for (i2 = 0, pFoo2 = pFOO->children; i2 < pFOO->count; pFoo2++, i2 += 1) {
					if (pFoo2->type != cbor2->type) continue;
					switch (cbor2->type) {
					case CN_CBOR_UINT:
						if ((group != 0) && (pFoo2->group != 0) && (pFoo2->group != group)) continue;
						if (pFoo2->value == cbor2->v.uint) i2 = pFOO->count + 1;
						break;

					case CN_CBOR_INT:
						if ((group != 0) && (pFoo2->group != 0) && (pFoo2->group != group)) continue;
						if (pFoo2->value == cbor2->v.sint) i2 = pFOO->count + 1;
						break;
					}

					if (i2 == pFOO->count + 1) break;
				}
				if (i2 == pFOO->count) pFoo2 = NULL;
			}
			if (i != 0) WrapPrintF(out, ", ");
			WrapPrintF(out, "\n");
			Indent(out, depth + 1);
			DumpTree(cbor2, out, pFoo2, depth + 1, true, false, fInComment);
			WrapPrintF(out, ":");
			cbor2 = cbor2->next;
			DumpTree(cbor2, out, pFoo2, depth + 1, false, true, fInComment);
		}
		if (i > 0) {
			WrapPrintF(out, "\n");
			Indent(out, depth);
		}
			WrapPrintF(out, "}");
		break;

	case CN_CBOR_BYTES:
		DumpBytes(out, cbor, depth);
		if ((pFOO != NULL) && (pFOO->children != NULL)) {
			const cn_cbor * cbor3 = cn_cbor_decode(cbor->v.bytes, cbor->length, NULL, NULL);
			if (cbor3 != NULL) {
				WrapPrintF(out, fInComment ? " \\ " : " / ");
				DumpTree(cbor3, out, pFOO->children, depth+1, true, true, true);
				WrapPrintF(out, fInComment ? " \\ " : " / ");
			}
		}
		break;

	case CN_CBOR_INT:
		WrapPrintF(out, "%d", cbor->v.sint);
		if (fValue && pFOO != NULL) {
			for (i = 0, pFoo2 = pFOO->children; i < pFOO->count; i++, pFoo2++) {
				if ((pFoo2->type == CN_CBOR_INT) && (pFoo2->value == cbor->v.sint)) {
					if (pFoo2->fieldName != NULL) {
						if (fInComment) WrapPrintF(out, " \\ %s \\", pFoo2->fieldName);
						else WrapPrintF(out, " / %s /", pFoo2->fieldName);
					}
					break;
				}
			}
		}
		break;

	case CN_CBOR_UINT:
		WrapPrintF(out, "%u", cbor->v.uint);
		if (fValue && (pFOO != NULL)) {
			for (i = 0, pFoo2 = pFOO->children; i < pFOO->count; i++, pFoo2++) {
				if ((pFoo2->type == CN_CBOR_UINT) && (pFoo2->value == cbor->v.uint)) {
					if (pFoo2->fieldName != NULL) {
						if (fInComment) WrapPrintF(out, " \\ %s \\", pFoo2->fieldName);
						else WrapPrintF(out, " / %s /", pFoo2->fieldName);
					}
					break;
				}
			}
		}
		break;

	case CN_CBOR_TEXT:
		WrapPrintF(out, "\"");
		for (i = 0; i < cbor->length; i++) WrapPrintF(out, "%c", cbor->v.str[i]);
		WrapPrintF(out, "\"");
		break;

	default:
		WrapPrintF(out, "##");
		break;
	}
}


int main(int argc, char ** argv)
{
	int			i;
    FILE *		in = NULL;
    FILE *		out = NULL;
    byte *      pb = NULL;
    size_t      cb = 0;
    byte        rgb[2048];
    size_t      cbIn;
	int			msg_type;
	int			forXML = false;
	FOO *		root = NULL;

    for (i=1; i<argc; i++) {
        if ((argv[i][0] == '-') || (argv[i][0] == '/')) {
            if (strcmp(&argv[i][1], "someoption") == 0) {
            }
			else if (strcmp(&argv[i][1], "xml=yes") == 0) {
				forXML = true;
			}
			else if (strcmp(&argv[i][1], "xml=no") == 0) forXML = false;
			else if (strncmp(&argv[i][1], "wrap=", 5) == 0) {
				WrapLineAt = atoi(&argv[i][6]);
			}
			else if (strncmp(&argv[i][1], "type=", 5) == 0) {
				if (strcmp(&argv[i][1], "type=encrypt") == 0) root = &EncryptedMessage;
				else if (strcmp(&argv[i][1], "type=envelope") == 0) root = &EnvelopedMessage;
				else if (strcmp(&argv[i][1], "type=signed") == 0) root = &SignedMessage;
				else if (strcmp(&argv[i][1], "type=mac") == 0) root = &MacMessage;
				else if (strcmp(&argv[i][1], "type=keyset") == 0) root = &KeySet;
				else if (strcmp(&argv[i][1], "type=key") == 0) root = &Key;
				else PrintUsage();
			}
            else {
                PrintUsage();
                exit(1);
            }
        }
        else {
            if (in == NULL) {
                in = fopen(argv[i], "rb");
                if (in == NULL) {
                    fprintf(stderr, "Unable to open file '%s'\n", argv[i]);
                    exit(1);
                }
            }
            else if (out == NULL) {
                out = fopen(argv[i], "wt");
                if (out == NULL) {
                    fprintf(stderr, "Unable to open file '%s'\n", argv[i]);
                    exit(1);
                }
            }
            else {
                PrintUsage();
                exit(1);
            }
        }
    }

    //  Set defaults

    if (in == NULL) {
#ifdef _WIN32
        in = stdin;
        _setmode(_fileno(stdin), _O_BINARY);
#else
        in = fdreopen(_fileno(stdin), NULL, O_RDONLY | OPEN_O_BINARY);
#endif
    }
    if (out == NULL) out = stdout;
    
    // Read the input to a buffer - needed for the parser

	cbIn = 1;
    while (cbIn > 0) {
        cbIn = fread(rgb, 1, sizeof(rgb), in);
        if (cbIn == 0) {
            //  finished
            break;
        }

        pb = realloc(pb, cb+cbIn);
        if (pb == NULL) {
            fprintf(stderr, "Error allocating memory\n");
            exit(1);
        }
        memcpy(pb+cb, rgb, cbIn);
        cb += cbIn;
    }

    //  Parse it

    cn_cbor * cbor = cn_cbor_decode(pb, cb, NULL, NULL);
	if (cbor == NULL) {
		fprintf(stderr, "Error parsing CBOR");
		exit(1);
	}

	if (root == NULL) {
		if (cbor->type == CN_CBOR_TAG) {
			switch (cbor->v.sint) {
			case 999: root = &SignedMessageWithTag; break;
			case 998: root = &EnvelopedMessageWithTag; break;
			case 997: root = &EncryptedMessageWithTag; break;
			case 996: root = &MacMessageWithTag; break;
			}
		}
	}

	if (forXML) {
		fprintf(out, "<t>Size of binary file is %d bytes</t>\n\n", cb);
		fprintf(out, "<figure><artwork type='CBORdiag'><![CDATA[\n");
	}

	DumpTree(cbor, out, root, 0, true, true, false);    
	WrapPrintF(out, "\n");

	if (forXML) {
		fprintf(out, "]]></artwork></figure>\n");
	}
}
