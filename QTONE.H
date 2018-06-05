CString WinInetErrCodToMessage(unsigned long errCode);
__declspec(dllexport) CString ObtenerContenidosONE(CString sIdContenido, CString sAmbiente);
__declspec(dllexport) CString ObtenerDescripcionID(int empieza, CString sRespuesta, bool sDatos);
__declspec(dllexport) CString ObtenerDocumentosCodigo(int empieza, CString sRespuesta);
__declspec(dllexport) CString RequestHTTPGET(CString sCodPath, CString sParametro, CString sAmbiente);
__declspec(dllexport) CString ObtenerURLdescarga(CString sAmbiente);
__declspec(dllexport) CString ObtenerSeparador();
__declspec(dllexport) CString ObtenerDivisor();
__declspec(dllexport) void MinToMay(char string[]);	
typedef  (CALLBACK* LPFNDLLFUNC)(char *, char *,char *);

