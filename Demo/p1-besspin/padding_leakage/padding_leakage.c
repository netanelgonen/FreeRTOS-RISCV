/* 
padding_leakage.c
Demonstrates information leakage through padding bytes in a struct.
*/

#include "padding_leakage.h"

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
   memory location. It sends a copy of that struct to Task2,
   which leads to leakage of the secret. */
void prvTask1( void *pvParameters ) {

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

	/* Write struct to allocated memory */ 
	if( struct_mem != NULL ) {
		struct_mem->a = 1;
		struct_mem->b = 2;
		struct_mem->c = 3;
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
	struct test lReceivedStruct;
	xStatus = xQueueReceive( xQueue, &lReceivedStruct, 0 );	

	/* Print struct, including padding bytes */ 
	if( xStatus == pdPASS ) {
		printBytes( ( &lReceivedStruct.b ), 4 );
	} else {
		printf( "Could not receive from the queue. \r\n" );
	}

	/* Delete task when done */
	vTaskDelete( NULL ); 
}
