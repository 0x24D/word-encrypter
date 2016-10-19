#include <conio.h>		// for kbhit
#include <iostream>		// for cin >> and cout <<
#include <iomanip>		// for fancy output
using namespace std;

#define MAXCHARS 6		// feel free to alter this, but 6 is the minimum
#define dollarchar '$'  // string terminator

char OChars[MAXCHARS],
	 EChars[MAXCHARS],
	 DChars[MAXCHARS];	// Global Original, Encrypted, Decrypted character strings

//----------------------------- C++ Functions ----------------------------------------------------------

void get_char (char& a_character)
{
	cin >> a_character;
	while (((a_character < '0') | (a_character > 'z')) && (a_character != dollarchar))
	{	cout << "Alphanumeric characters only, please try again > ";
		cin >> a_character;
	}
}
//-------------------------------------------------------------------------------------------------------------

void get_original_chars (int& length)
{	char next_char;
	length = 0;
	get_char (next_char);

	while ((length < MAXCHARS) && (next_char != dollarchar))
	{
		OChars [length++] = next_char;
		get_char (next_char);
	}
}

//---------------------------------------------------------------------------------------------------------------
//----------------- ENCRYPTION ROUTINES -------------------------------------------------------------------------

void encrypt_chars (int length, char EKey)
{   char temp_char;						//Character temporary store

	for (int i = 0; i < length; i++)	//Encrypt characters one at a time
	{
		//mov ecx,6						//Set ECX register to 6. Used as loop counter.
		//encryptLoop:					//Label to be used to loop the encryption sequence.
		temp_char = OChars [i];			//Get the next char from Original Chars array
		
		__asm{
			push	eax					//Save value stored in EAX, will be restored after the routine is completed.
			push	ecx					//Save value stored in ECX, will be restored after the routine is completed.
			movzx	ecx, temp_char		//Move the character to be encrypted into ECX, fill the empty space in ECX with zeros (to prevent errors).
			lea		eax, EKey			//Move the address of the EKey into the EAX register, quicker than moving the EKey using the move instruction.
			///
			push	ecx					//Save the character to be encrypted onto the stack.
			mov	ecx, [eax]				//Move the Ekey into ECX register.
			and	ecx, 0x000000A7			//And EKey with A7 hexadecimal.
			rol	cl, 1					//Rotate EKey bits to left once, bits that are removed on the left are appended to the right side.
			add	ecx, 0x03				//Add 3 hexadecimal to altered EKey.
			mov	edx, ecx				//Move altered EKey into EDX register.
			pop	ecx						//Retrieve the character to be encrypted, from the stack, into ECX register.
			add	ecx, edx				//Add altered EKey to character to be encrypted, original EKey (in EDX) stays the same.
			ror	cl, 2					//Rotate character bits to right twice, bits that are removed on the right are appended to the left side.
			mov[eax], edx				//Move altered Ekey to the memory address which is stored in EAX register.
			mov	eax, ecx				//Move character to be encrypted into EAX register.
			add	eax, 0x10				//Add 10 hexadecimal to character to be encrypted. Character has been encrypted.
			///
			mov		temp_char, al		//Move encrypted character into the last 8 bytes of the EAX register.
			pop		ecx					//Retrieve original value of ECX from the stack.
			pop		eax					//Retrieve original value of EAX from the stack.
	}
		EChars [i] = temp_char;			//Store encrypted char in the Encrypted Chars array
		//loop encryptLoop				//Decrement the loop counter (ECX) by 1 and go to encryptLoop label.
	}
   return;
	//--- End of Assembly code
}
// end of encrypt_chars function
//---------------------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------------------------
//----------------- DECRYPTION ROUTINES -------------------------------------------------------------------------
//
void decrypt_chars (int length, char EKey)
{	char temp_char;						//Character temporary store

	for (int i = 0; i < length; i++)	//Decrypt characters one at a time
	{
		temp_char = EChars[i];			//Get the next char from Encrypted Chars array
		__asm {
			push	eax					//Save value stored in EAX, will be restored after call is completed.
			push	ecx					//Save value stored in ECX, will be restored after call is completed.
			movzx	ecx, temp_char		//Move the character to be encrypted into ECX, fill the empty space in ECX with zeros (to prevent errors).
			lea		eax, EKey			//Move the address of the EKey into the EAX register, quicker than moving the EKey using the move instruction.
			///
			push	ecx					//Save the character to be decrypted onto the stack.
			mov     ecx, [eax]			//Move the Ekey into ECX register.
			and     ecx, 0x000000A7		//And EKey with A7 hexadecimal.
			rol     cl, 1				//Rotate EKey bits to left once, bits that are removed on the left are appended to the right side.
			add     ecx, 0x03			//Add 3 decimal to altered EKey.
			mov     edx, ecx			//Move altered EKey into EDX register.
			mov		[eax], edx			//Move altered Ekey to the memory address which is stored in EAX register.
			pop		eax					//Retrieve the character to be decrypted, from the stack, into EAX register.
			sub     eax, 0x10			//Subtract 10 hexadecimal from character to be decrypted.
			rol     al, 2				//Rotate Character bits to left twice, bits that are removed on the left are appended to the right side.
			sub     eax, edx			//Subtract altered EKey from character to be decrypted, original EKey (in EDX) stays the same.
			cmp		eax, 0x61			//Check to see if decrypted letter is in lowercase.
			jge		lowercase			//If the character is lowercase, go to lowercase label.
			add		eax, 0x20			//Add 20 hexadecimal to decrypted character, makes it lowercase.
			///
lowercase:	mov		temp_char, al		//Move encrypted character into the last 8 bytes of the EAX register.
			pop		ecx					//Retrieve original value of ECX from the stack.
			pop		eax					//Retrieve original value of EAX from the stack.
	}
	DChars[i] = temp_char;				// Store decrypted char in the Decrypted Chars array
	}
	return;
}
// end of decrypt_chars function
//---------------------------------------------------------------------------------------------------------------

int main(void)
{
	int  char_count;	// The number of actual characters entered (upto MAXCHARS limit).
	char EKey;			// Encryption key.

		cout << "\nPlease enter your Encryption Key (EKey) letter: "; get_char (EKey);

		cout << "\nNow enter upto " << MAXCHARS << " alphanumeric characters:\n";
		get_original_chars (char_count);
		cout << "\n\nOriginal source string = " << OChars << "\tHex = ";
		for (int i=0; i<char_count; i++) cout<<hex<<setw(2)<<setfill('0')<< ((int (OChars[i]))&0xFF)<<"  ";

		encrypt_chars (char_count, EKey);
		cout << "\n\nEncrypted string       = " << EChars << "\tHex = ";
		for (int i=0; i<char_count; i++) cout<< ((int (EChars[i]))&0xFF)<<"  ";

		decrypt_chars (char_count, EKey);
		cout << "\n\nDecrypted string       = " << DChars << "\tHex = ";
		for (int i=0; i<char_count; i++) cout<< ((int (DChars[i]))&0xFF)<<"  ";

	cout << "\n\nPress a key to end...";
	while ( !_kbhit());					//hold the screen until a key is pressed
	return (0);


} // end of whole encryption/decryption program --------------------------------------------------------------------