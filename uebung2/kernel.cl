__kernel void matrixVectorMulti(int size,__constant int * vectorA, __constant int * vectorB, __global int * resultVector){

	for( int zeile = 0; zeile < size; ++zeile ) {
        for( int spalte = 0; spalte < size; ++spalte ) {
            resultVector[zeile] += vectorA[spalte + zeile * size] * vectorB[spalte];
        }
    }
}