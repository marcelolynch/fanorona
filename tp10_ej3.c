#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BLOQUE 5

void * asignarMem (void *v, int dim, size_t bytes, size_t bloque) {
	void *aux;
	if (dim % bloque == 0) {
		aux = realloc(v, (dim+bloque)*bytes);
		if (aux == NULL) {
			free (v);
			return NULL;
		}
		else
			v = aux;
	}
	return v;
}

void * unionOrdenada (const void *v1, int dim1, const void *v2, int dim2, size_t bytes, int *nuevaDim, int (*cmp) (const void*, const void*)) {
	void *nuevoVec = NULL, *aux;
	int i,j,k;
	int comp;
	i=j=k=0;
	
	while (i<dim1 && j<dim2) {
		nuevoVec = asignarMem (nuevoVec, k, bytes, BLOQUE);

		if (nuevoVec == NULL)
			return NULL;

		/* saltear repetidos */
		while (i < dim1-1 && cmp(v1+i*bytes, v1+(i+1)*bytes) == 0)
			i++;

		while (j < dim2-1 && cmp(v2+j*bytes, v2+(j+1)*bytes) == 0)
			j++;	

		comp = cmp(v1+i*bytes, v2+j*bytes);

		if (comp > 0) {
			memcpy(nuevoVec+k*bytes, v2+j*bytes, bytes);
			j++; k++;
		}
		else if (comp < 0) {
			memcpy(nuevoVec+k*bytes, v1+i*bytes, bytes);
			i++; k++;
		}
		else /* son iguales */ {
			memcpy(nuevoVec+k*bytes, v1+i*bytes, bytes);
			i++; j++; k++;
		}
	}

	while (i < dim1) {
		nuevoVec = asignarMem (nuevoVec, k, bytes, BLOQUE);
		if (nuevoVec == NULL)
			return NULL;
		while (i < dim1-1 && cmp(v1+i*bytes, v1+(i+1)*bytes) == 0)
			i++;	
		memcpy(nuevoVec+k*bytes, v1+i*bytes, bytes);
		i++; k++;
	}

	while (j < dim2) {
		nuevoVec = asignarMem (nuevoVec, k, bytes, BLOQUE);
		if (nuevoVec == NULL)
			return NULL;
		while (j < dim2-1 && cmp(v2+j*bytes, v2+(j+1)*bytes) == 0)
			j++;	
		memcpy(nuevoVec+k*bytes, v2+j*bytes, bytes);
		j++; k++;
	}

	nuevoVec = realloc(nuevoVec, k*bytes);
	*nuevaDim = k;
	return nuevoVec;
}

int intCmp (int *a, int *b) {
	return *a - *b;
}

main (void) {
	int v1[] = {2, 2, 3, 4, 4, 5, 5, 5, 5, 5};
	int v2[] = {2, 2, 2, 3, 4, 6, 7, 9};
	int nuevaDim;
	int *vec = unionOrdenada (v1, 9, v2, 8, sizeof(int), &nuevaDim, (int (*) (const void*,const void*))intCmp);
	int i;

	for (i=0; i<nuevaDim; i++)
		printf("%d\n", vec[i]);
	return 0;
}
