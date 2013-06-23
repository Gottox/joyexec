/* Plan9-style Argument parsing */
/* Vars: _c -> count; _b -> break; _a -> argument */
#define ARG int _c, _b; char *_a; \
	for(_c = 1; _c < argc && argv[_c][0] == '-' && argv[_c][1] && \
			(strcmp(argv[_c], "--") != 0); _c++) \
		for(_a = &argv[_c][1], _b = 0; !_b && *_a; _a++ ) \
			switch(*_a)
#define ARGVAL()	(!_b && _a[1] && (_b = 1) ? &_a[1] : _c + 1 == argc ? \
		0 : argv[++_c])
#define ARGCHR()	(*_a)
#define ARGC()		_c

