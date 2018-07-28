#pragma comment(lib, "crypt32.lib")

#include <stdio.h>
#include <windows.h>
#include <Wincrypt.h>
#define MY_ENCODING_TYPE  (PKCS_7_ASN_ENCODING | X509_ASN_ENCODING)

char* Decrypt(BYTE * pbDataInput, int length)
{

	// Copyright (C) Microsoft.  All rights reserved.
	// Encrypt data from DATA_BLOB DataIn to DATA_BLOB DataOut.
	// Then decrypt to DATA_BLOB DataVerify.

	//-------------------------------------------------------------------
	// Declare and initialize variables.

	DATA_BLOB DataIn;
	DATA_BLOB DataVerify;
	// DWORD cbDataInput = strlen((char *)pbDataInput) + 1;
	DataIn.pbData = pbDataInput;
	DataIn.cbData = length; //cbDataInput;
	CRYPTPROTECT_PROMPTSTRUCT PromptStruct;
	LPWSTR pDescrOut = NULL;

	//-------------------------------------------------------------------
	//  Begin processing.

	printf("The data length to be encrypted is: %d\n", length);

	//-------------------------------------------------------------------
	//  Initialize PromptStruct.

	ZeroMemory(&PromptStruct, sizeof(PromptStruct));
	PromptStruct.cbSize = sizeof(PromptStruct);
	PromptStruct.dwPromptFlags = CRYPTPROTECT_PROMPT_ON_PROTECT;
	PromptStruct.szPrompt = L"This is a user prompt.";

	//-------------------------------------------------------------------
	//   Begin unprotect phase.

	if (CryptUnprotectData(
		&DataIn,
		&pDescrOut,
		NULL,                 // Optional entropy
		NULL,                 // Reserved
		&PromptStruct,        // Optional PromptStruct
		0,
		&DataVerify))
	{
		DataVerify.pbData[DataVerify.cbData] = 0;
		printf("The decrypted data is: %s\n", DataVerify.pbData);
		printf("The description of the data was: %S\n", pDescrOut);
	}
	else
	{
		return "!!! Decryption error !!!!";
	}
	//-------------------------------------------------------------------
	// At this point, memcmp could be used to compare DataIn.pbData and 
	// DataVerify.pbDate for equality. If the two functions worked
	// correctly, the two byte strings are identical. 

	//-------------------------------------------------------------------
	//  Clean up.

	LocalFree(pDescrOut);
	// LocalFree(DataIn.pbData);
	// LocalFree(DataVerify.pbData);


	return (char*)DataVerify.pbData;
} // End of main

