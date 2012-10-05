#ifndef EAN13
#define EAN13

#define ENCODED_NUMBER_SIZE		12
#define ENCODED_BUFFER_SIZE		(ENCODED_NUMBER_SIZE + 1)
#define DECODED_NUMBER_SIZE		13
#define DECODED_BUFFER_SIZE		(DECODED_NUMBER_SIZE + 1)

/*************************************************************************
* Function:		Ean13_Encode
* Description:	Encode a string that contains the number to a char array
* Input:		str		- a string that contains 13 digit number, e.g. 
*						"1234567890123"
* Output:		encoded	- the encoded buffer (12 chars)
*************************************************************************/
int Ean13_Encode(const char str[DECODED_BUFFER_SIZE], char encoded[ENCODED_BUFFER_SIZE]);

/*************************************************************************
* Function:		Ean13_Decode
* Description:	Decode a barcode buffer and return a string representation
*				of the number
* Input:		barcode	- an encoded buffer that contains 12 chars
* Output:		decoded	- the decoded number as a string
*************************************************************************/
int Ean13_Decode(const char barcode[ENCODED_BUFFER_SIZE], char decoded[DECODED_BUFFER_SIZE]);

#endif //EAN13