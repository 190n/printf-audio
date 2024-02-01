#include <printf.h>
#include <stdio.h>

typedef struct {
	int monkey;
} Foo;

int custom_specifier(FILE *stream, const struct printf_info *info, const void *const *args) {
	(void) info;
	Foo *arg = **(Foo ***) args;
	return fprintf(stream, "Foo@%p { monkey = %d }", (void *) arg, arg->monkey);
}

int custom_arginfo(const struct printf_info *info, size_t n, int *argtypes, int *size) {
	(void) info;
	if (n > 0) {
		argtypes[0] = PA_POINTER;
		*size = sizeof(void *);
	}
	return 1;
}

int main(void) {
	register_printf_specifier('A', custom_specifier, custom_arginfo);
	Foo xyz = { .monkey = 5 };
	printf("%A\n", &xyz);
	return 0;
}
