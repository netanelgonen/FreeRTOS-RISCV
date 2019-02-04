/* 
patched_padding_leakage.c
Patched version of padding_leakage.c, which demonstrated information
leakage through padding bytes in a struct. This version eleminates
leakage by sending a copy of the struct that only includes the elements
without any padding.
*/

#include "patched_padding_leakage.h"

/* Used to check processor-specific alignment
   Only used for debugging. */
void vTaskCheck( void *pvParameters ) {
	
	printf( "Alignment of char = %zu\n", alignof( char ) );
	printf( "Alignment of int = %zu\n", alignof( char ) );
	printf( "Alignment of struct = %zu\n", alignof( struct { int a; char b; int c; } ) );
}

/* Print bytes, used to demonstrate leakage in char padding */
void printBytes( void *ptr, int size ) {
		
	unsigned char *p = ptr;
	int i;
	for( i=(size-1); i>=0; i-- ) {
		printf( "%02hhX", p[i] );
	} 
	printf("\n");
}

/* Task1 writes secret to memory, then writes a struct to the same
   memory location. It sends a copy of the elements of that struct
   without any padding bytes to Task2, which leads to leakage of 
   the secret. */
void vTask1( void *pvParameters ) {

	/* Status of queue */
	BaseType_t xStatus;

	/* Allocate memory for secret */ 
	int* secret_mem = pvPortMalloc( 3 * sizeof( int ) );

		
	/* Write secret */
	int secret[3] = { 0xffffffff, 0xffffffff, 0xffffffff };	
	if( secret_mem != NULL ) {
		secret_mem = secret;
	} else {
		printf( "Memory could not be allocated for secret\n" );
		vTaskDelete( NULL );
	}	

	/* Check that secret is same size as struct test */	
	assert( sizeof( secret ) == sizeof( struct test ) );

	/* Cast pointer from int pointer to struct pointer */ 
	struct test* struct_mem = (struct test *) secret_mem;

	/* Check for intermediate padding */
	static_assert( offsetof( struct test, c ) == 
			offsetof( struct test, padding_3 ) + 1,
			"Struct contains intermediate padding" );

	/* Check for trailing padding */
	static_assert( sizeof( struct test ) == 
			offsetof( struct test, c ) + sizeof( int ),
			"Struct contains trailing padding" );

	/* Write struct to allocated memory */ 
	if( struct_mem != NULL ) {
		struct_mem->a = 1;
		struct_mem->b = 2;
		struct_mem->c = 3;
		struct_mem->padding_1 = 0;
		struct_mem->padding_2 = 0;
		struct_mem->padding_3 = 0;
	} else {
		printf( "Memory could not be allocated for struct\n" );
		vTaskDelete( NULL );
	}

	/* Send data to Task2 using queue, which queues by copy */
	xStatus = xQueueSend( xQueue, struct_mem, 0 );	
	if( xStatus != pdPASS ) {
		printf( "Could not send to the queue. \r\n" );
	}

	/* Get priority of Task1 */ 
	UBaseType_t uxPriority;
	uxPriority = uxTaskPriorityGet( NULL );

	/* Set Task2 priority to above Task1 priority */
	vTaskPrioritySet( xTask2Handle, (uxPriority + 1) );

	/* Delete task when done */
	vTaskDelete( NULL );

}

/* Task2 receives struct and reveals leakage through padding bytes */
void vTask2 ( void *pvParameters ) {

	/* Status of queue */
	BaseType_t xStatus;
	
	/* Receive struct from Task1 through queue */ 
	
	// vv should this be malloc instead?
	struct test lReceivedStruct;
	xStatus = xQueueReceive( xQueue, &lReceivedStruct, 0 );	

	/* Print struct, including padding bytes */ 
	if( xStatus == pdPASS ) {
		printBytes( ( &lReceivedStruct.b ), 4 );
	} else {
		printf( "Could not receive from the queue. \r\n" );
	}	

	vTaskDelete( NULL );
}
