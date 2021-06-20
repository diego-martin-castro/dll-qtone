#include "stdafx.h"
#include "QTONE.h"
#include "QTDIPCall.h"
#include "QTClass.h"
#include "QTApplut.h"

#define EXPORTING_DLL

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved )
{
    return TRUE;
} 

//DEFINES
#define TBL_PATH			"QTPATHAPLICATIVOS"
#define SEPARADOR			"|#|"
#define DIV					"|&|"
#define HTTP				7
#define HTTPS				8
#define TAG_CONTENIDOS		"<CONTENIDOS>"
#define TAG_TIPOSCONTENIDOS "<TIPOSCONTENIDO>"
#define TAG_DESCRIPCION		"<DESCRIPCION>"
#define TAG_ARCHIVO			"<NOMBREARCHIVO>"
#define TAG_CATEGORIA		"<CATEGORIA>"
#define TAG_BARRACATEGORIA  "</CATEGORIA>"
#define TAG_CODIGO			"<CODIGO>"
#define TAG_TIPO			"<TIPO>"
#define TAG_ID				"<ID>"
#define	PREF_CAT			"OT"
#define PREF_DOC			"OC"	
#define PREF_ADJ			"OA"	

//Errores
#define ERR1				"No se reconoce el ambiente de ejecución"
#define ERR2				"No se reconoce el area solicitada"
#define ERR3				"No se puede conectar con el servicio"
#define ERR4				"Error en lectura del XML"

/////////////////////////////////////////////////////////////////////////////
// OPERACIONES
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CONSULTA DE CONTENIDOS 
/////////////////////////////////////////////////////////////////////////////

//Funcion para obenter un String FINAL con IDcategoria + NOMcategoria + IDdoc + DesDOC / separados por |#| (linea) y |&| Dato
CString ObtenerContenidosONE(CString sIdContenido, CString sAmbiente)
{	
	//Variables
	CString sLinea = "";
	CString sIDCat = "";
	CString sNomCat = "";
	CString sDivSep = "";
	CString sDatosDoc = ""; 
	CString sAuxDivSep = "";
	CString sRespuestaCat = "";
	CString sRespuestaDoc = "";
	
	//Obtener la longitud de los separadores seteados
	sAuxDivSep = SEPARADOR;
	int iSepLong = sAuxDivSep.GetLength();

	sAuxDivSep = DIV;
	int iDivLong = sAuxDivSep.GetLength();

	//Controlar la definicion de un ambiente
	if(sAmbiente == "")
		//No se definio el ambiente
		return ERR1;  

	//Controlar la definicion del contenido solicitado
	if(sIdContenido == "")	
		return ERR2;
	
	//DebugBreak();

	//HACER REQUEST CATEGORIAS
	sRespuestaCat = RequestHTTPGET(PREF_CAT, sIdContenido, sAmbiente);

	//Retornar error del servicio o esta mal el parametro del categoria padre, no trae las categorias
	if(sRespuestaCat == "")
		return ERR3;	
			
	//Obetner el ID de las categorias
	CString sIDCategorias;	
	sIDCategorias = ObtenerDescripcionID(0, sRespuestaCat, true);

	//Obtener el nombre de las categorias
	CString sNombreCategorias;			
	sNombreCategorias = ObtenerDescripcionID(0, sRespuestaCat, false);
	
	//Recorrer los ID de las categorias para encontrar los documentos
	while(sIDCategorias.Find(SEPARADOR) != -1)
	{				
		sIDCat = sIDCategorias.Mid(0, sIDCategorias.Find(SEPARADOR));
		sIDCategorias = sIDCategorias.Mid(sIDCategorias.Find(SEPARADOR), sIDCategorias.GetLength());
		sIDCategorias = sIDCategorias.Mid(iSepLong, sIDCategorias.GetLength());

		sNomCat = sNombreCategorias.Mid(0, sNombreCategorias.Find(SEPARADOR));
		sNombreCategorias = sNombreCategorias.Mid(sNombreCategorias.Find(SEPARADOR), sNombreCategorias.GetLength());
		sNombreCategorias = sNombreCategorias.Mid(iSepLong, sNombreCategorias.GetLength());
		
		//Obtener datos de los documentos asociados a la categoria 
		CString sDatosDocumentos;
		
		//HACER REQUEST DOCUMENTOS
		sRespuestaDoc = RequestHTTPGET(PREF_DOC, sIDCat, sAmbiente);
		
		//Controlar que la categoria no este vacia, ya que puede no tener documentos, por lo tanto, no se muestra
		if(sRespuestaDoc != "") 
		{		
			if(sRespuestaDoc.Find(TAG_CONTENIDOS) != -1)
			{
				sDatosDocumentos = ObtenerDocumentosCodigo(0, sRespuestaDoc);
				
				//Parear los documentos
				while(sDatosDocumentos.Find(SEPARADOR) != -1)
				{	
					sDatosDoc = sDatosDocumentos.Mid(0, sDatosDocumentos.Find(SEPARADOR));
					sDatosDocumentos = sDatosDocumentos.Mid(sDatosDocumentos.Find(SEPARADOR), sDatosDocumentos.GetLength());
					sDatosDocumentos = sDatosDocumentos.Mid(iSepLong, sDatosDocumentos.GetLength());
					
					if(sDatosDoc != "")
					{
						//Separar la descripcion asi queda primero
						CString sDescDoc = "";
						sDescDoc = sDatosDoc.Mid(0, sDatosDoc.Find(DIV));
						sDatosDoc = sDatosDoc.Mid(sDatosDoc.Find(DIV), sDatosDoc.GetLength());
						sDatosDoc = sDatosDoc.Mid(iDivLong, sDatosDoc.GetLength());
						
						//Armar el retorno
						sLinea = sLinea + sDescDoc + DIV + sNomCat + DIV + sDatosDoc + DIV + SEPARADOR; 
					}
				}
			}
		}	
	}
	
	//Verificar que el XML no venga vacio 
	if(sLinea == "")
		sLinea = ERR4;
		
	return sLinea;
}

// Funcion para obener el ID o la Descripcion de los contenidos segun el ID de area que se pase como parametro
CString ObtenerDescripcionID(int empieza, CString sRespuesta, bool sDatos)
{	
	//Variables			
	CString sID = "";
	CString sIDCat = "";
	CString sCategoria = "";
	CString sCategorias = "";
	CString sDescripcion = "";
	CString sParteRespuesta = "";

	int i = 0;
	int indice = 0;		 
	int EncontroID = 0;
	int EncontroCATEGORIA = 0;
	int EncontroDESCRIPCION = 0;
	int longitudRes = sRespuesta.GetLength();
	int longitudPartRes = 0;
	int sFinCatRes = 0;
	int sIniCatRes = 0;
	int sParteCatRes = 0;
 
	char id[1000];
	char idcat[1000];
	char categoria[1000];
	char descripcion[1000];

	memset(&id, 0x00, sizeof(id));
	memset(&idcat, 0x00, sizeof(idcat));
	memset(&categoria, 0x00, sizeof(categoria));
	memset(&descripcion, 0x00, sizeof(descripcion));	

	while(sRespuesta.Find(TAG_CATEGORIA) != -1)
	{				
		sFinCatRes = sRespuesta.Find(TAG_BARRACATEGORIA) + 12;
		sIniCatRes = sRespuesta.Find(TAG_CATEGORIA);
		
		sParteRespuesta= sRespuesta.Mid(sIniCatRes, sFinCatRes - sIniCatRes);
		longitudPartRes = sParteRespuesta.GetLength();		
		
		while(sParteRespuesta.Find(TAG_DESCRIPCION) != -1)
		{
			//## CATEGORIAS PRINCIPALES ##//

			//Vaciar variables en memomria
			memset(&idcat,0x00,	sizeof(idcat));
			memset(&categoria,0x00,	sizeof(categoria));	
			
			//Vaciar variables
			sIDCat = "";
			sCategoria = "";

			//Reasignar indice a cero
			indice = 0;

			//Buscar el "ID" en la respuesta
			EncontroID = sParteRespuesta.Find(TAG_ID);

			//Si lo encuentra
			if(EncontroID != -1)
			{
				//Armar el ID
				for(int i=EncontroID + 4; i<=longitudPartRes; i++)
				{
					if(sParteRespuesta[i] != '<')	
					{
						idcat[indice] = sParteRespuesta[i];				
						indice++;
					}
					else
						break;
				}			
				//Asignar el ID encontrado
				sIDCat = idcat;
			}
			else
				//NO hay un ID en la respuesta, devolver error
				return ERR4;

			//Reasignar indice a cero
			indice = 0;

			//Buscar la "CATEGORIA" en la respuesta
			EncontroCATEGORIA = sParteRespuesta.Find(TAG_DESCRIPCION);

			//Si lo encuentra
			if(EncontroCATEGORIA != -1)
			{
				//Armar el ID
				for(int i=EncontroCATEGORIA + 13; i<=longitudPartRes; i++)
				{
					if(sParteRespuesta[i] != '<')	
					{
						categoria[indice] = sParteRespuesta[i];				
						indice++;
						empieza = i;
					}
					else
						break;
				}			
				//Asignar el ID encontrado
				sCategoria = categoria;
			}
			else
				//NO hay una CATEGORIA ID en la respuesta, devolver error
				return ERR4;	
			
			//## SUBCATEGORIAS ##//

			//Agregar la categoria principal pq puede tener documentos 
			//Armar respuesta para tener en cuenta las categorias principales con docs
			if(sDatos) 
				sCategorias = sCategorias +  SEPARADOR + sIDCat;
			else
				sCategorias = sCategorias +  SEPARADOR + sCategoria + DIV + "   ";				

			if(sParteRespuesta.Find(TAG_TIPO) != -1)
				sParteRespuesta = sParteRespuesta.Mid(empieza, sParteRespuesta.GetLength() - empieza);
			
			//BUSCAR LOS TIPOS
			while(sParteRespuesta.Find(TAG_TIPO) != -1)
			{
				//Vaciar variables en momeria
				memset(&id,			0x00,	sizeof(id));			
				memset(&descripcion,0x00,	sizeof(descripcion));
				
				//Vaciar variables 
				sID = "";
				sDescripcion = ""; 

				//Reasignar indice a cero
				indice = 0;

				//Buscar el "ID" en la respuesta
				EncontroID = sParteRespuesta.Find(TAG_ID);

				//Si lo encuentra
				if(EncontroID != -1)
				{
					//Armar el ID
					for(int i=EncontroID + 4; i<=longitudPartRes; i++)
					{
						if(sParteRespuesta[i] != '<')	
						{
							id[indice] = sParteRespuesta[i];				
							indice++;
						}
						else
							break;
					}			
					//Asignar el ID encontrado
					sID = id;
				}
				else
					//NO hay un ID en la respuesta, devolver error
					return ERR4;

				//Reasignar indice a cero
				indice = 0;

				//Buscar "DESCRIPCION" en la respuesta
				EncontroDESCRIPCION = sParteRespuesta.Find(TAG_DESCRIPCION);
				
				//Si lo encuentra
				if(EncontroDESCRIPCION != -1)
				{
					//Armar DESCRIPCION
					for(int i=EncontroDESCRIPCION + 13; i<=longitudPartRes; i++)
					{
						if(sParteRespuesta[i] != '<')	
						{
							descripcion[indice] = sParteRespuesta[i];				
							indice++;
							empieza = i; 
						}
						else
							break;
					}			
					//Asignar el DESCRIPCION encontrado
					sDescripcion = descripcion;
				}
				else
					//NO hay un DESCRIPCION en la respuesta, devolcer error
					return ERR4;

				//Armar el retorno con los datos de la subcategorias inclusive
				if(sDatos) 
					sCategorias = sCategorias +  SEPARADOR + sID;		
				else
					sCategorias = sCategorias +  SEPARADOR + sCategoria + DIV + sDescripcion;

				sParteRespuesta = sParteRespuesta.Mid(empieza, sParteRespuesta.GetLength() - empieza);	
			}
			
			sParteRespuesta = sParteRespuesta.Mid(empieza, sParteRespuesta.GetLength() - empieza);
		}	
				
		sRespuesta = sRespuesta.Mid(sFinCatRes, longitudRes);
	}	
	
	//Verificar que no este vacia y agregar separador al comienzo
	if(sCategorias != "")
		sCategorias = sCategorias + SEPARADOR;
	
	//Retornar String con categorias o id
	return sCategorias;
}

//Funcion para obtener los codigos y documentos segun la categoria que se pase como parametro
CString ObtenerDocumentosCodigo(int empieza, CString sRespuesta)
{	
	//Variables			
	CString sCodigo = "";
	CString sArchivo = "";
	CString sDocumento = "";
	CString sDescripcion = "";

	int i = 0;
	int indice = 0;		 
	int EncontroCODIGO = 0;
	int EncontroARCHIVO = 0;
	int EncontroDESCRIPCION = 0;

	int longitudRes = sRespuesta.GetLength();

	char codigo[1000];
	char archivo[1000];
	char descripcion[1000];

	while(sRespuesta.Find(TAG_DESCRIPCION) != -1)
	{	
		//Vaciar reserva de memoria CODIGO
		memset(&codigo,		0x00,	sizeof(codigo));

		//Vaciar CODIGO
		sCodigo = "";

		//Reasignar indice a cero
		indice = 0;

		//Buscar el "CODIGO" en la respuesta		
		EncontroCODIGO = sRespuesta.Find(TAG_CODIGO);

		//Si lo encuentra
		if(EncontroCODIGO != -1)
		{
			//Armar el CODIGO
			for(int i=EncontroCODIGO + 8; i<=longitudRes; i++)
			{
				if(sRespuesta[i] != '<')	
				{
					codigo[indice] = sRespuesta[i];				
					indice++;
				}
				else
					break;
			}			
			//Asignar el CODIGO encontrado
			sCodigo = codigo;
		}
		else
			//NO hay un CODIGO en la respuesta, devolcer error
			return ERR4;

		//Reasignar indice a cero
		indice = 0;

		//Buscar "DESCRIPCION" en la respuesta	
		EncontroDESCRIPCION = sRespuesta.Find(TAG_DESCRIPCION);
	
		//Vaciar DESCRIPCION
		sDescripcion = "";
		
		//Si lo encuentra
		if(EncontroDESCRIPCION != -1)
		{
			//Vaciar reserva de memoria DESCRIPCION
			memset(&descripcion,0x00,	sizeof(descripcion));

			//Armar DESCRIPCION
			for(int i=EncontroDESCRIPCION + 13; i<=longitudRes; i++)
			{
				if(sRespuesta[i] != '<' && sRespuesta[i+1] != '/')	
				{
					descripcion[indice] = sRespuesta[i];				
					indice++;
					empieza = i; 
				}
				else
					break;
			}			
			//Asignar el DESCRIPCION encontrado
			sDescripcion = descripcion;
		}
		else
			//NO hay un tag DESCRIPCION en la respuesta, devolver error
			return ERR4;

		//Reasignar indice a cero
		indice = 0;

		//Buscar "ARCHIVO" en la respuesta	
		EncontroARCHIVO = sRespuesta.Find(TAG_ARCHIVO);
		
		//Vaciar ARCHIVO
		sArchivo = "";

		//Si lo encuentra
		if(EncontroARCHIVO != -1)
		{
			//Vaciar reserva de memoria ARCHIVO
			memset(&archivo,0x00,	sizeof(archivo));

			//Armar DESCRIPCION
			for(int i=EncontroARCHIVO + 15; i<=longitudRes; i++)
			{
				if(sRespuesta[i] != '<')	
				{
					archivo[indice] = sRespuesta[i];				
					indice++;
					empieza = i; 
				}
				else
					break;
			}			
			//Asignar el ARCHIVO encontrado
			sArchivo = archivo;
		}

		//Armar el retorno		
		//Verificar que haya un archivo fisico cargado y no solo la entrada
		if(sArchivo != "")
			sDocumento = sDocumento + SEPARADOR + sDescripcion + DIV + sArchivo + DIV + sCodigo;
				
		sRespuesta = sRespuesta.Mid(empieza, longitudRes);				
	}

	//Verificar que no este vacia y agregar separador al comienzo
	if(sDocumento != "")
		sDocumento = sDocumento + SEPARADOR;
	
	//Retornar String con codigo + documentos
	return sDocumento;
}

//Funcion para obtener la URL de descarga de los documentos
CString ObtenerURLdescarga(CString sAmbiente)
{	
	CString sUrl = "";
	CString sCodPath = PREF_ADJ;
	CString sDominio = sCodPath + sAmbiente;

	//Obtener de la tabla QTPATHAPLICATIVOS la url de ONE Contenidos
	QTGetCTEntry(TBL_PATH, QT_COMBO_GET_DECODE, sDominio, sUrl);
	sUrl.TrimRight();
	sUrl.TrimLeft();
	
	return sUrl;
}

//Funcion para obtener el separador por cada linea para su parseo
CString ObtenerSeparador()
{
	return SEPARADOR;
}

//Funcion para obtener el divisor por cada data de una linea para su parseo
CString ObtenerDivisor()
{
	return DIV;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Funcion para hacer los diferentes request a las URL (Generica)
CString RequestHTTPGET(CString sCodPath, CString sParametro, CString sAmbiente)
{
	//DebugBreak();
	//Estructuras
	_SEND_ONE	cSend;
	_RETURN_ONE	cReturn;
	
	//Variables	
	int	iRet=0;
	int uriPosition = 0;
	int longitudUrl = 0;

	char respuesta[10000];

	memset(&respuesta,	0x00,	sizeof(respuesta));
	memset(&cSend,		0x00,	sizeof(cSend));
	memset(&cReturn,	0x00,	sizeof(cReturn));
	
	CString sDominio = sCodPath + sAmbiente;
	CString sUrl = "";
	CString sRespuesta = "";
	CString	port = "80";
	CString ssl = "1";
	CString sData = "";
	CString criterioXML = ""; 
	CString url = "";	
	CString uri = "";
	CString uriAux = "";	
	
	//Obtener de la tabla QTPATHAPLICATIVOS la url de ONE Contenidos
	QTGetCTEntry(TBL_PATH, QT_COMBO_GET_DECODE, sDominio, sUrl);
	sUrl.TrimRight();

	//Parsear la url para obtener el Domain y el URI
	longitudUrl = sUrl.GetLength();
	
	if(sUrl.Find("http://") != -1)
		url = sUrl.Mid(HTTP, longitudUrl);

	if(sUrl.Find("https://") != -1)
		url = sUrl.Mid(HTTPS, longitudUrl);

	longitudUrl = url.GetLength();
	
	uriPosition = url.Find("/");
	
	uri = url.Mid(uriPosition, longitudUrl);	
	uri = uri + sParametro;
	url = url.Mid(0, uriPosition);		

	//Armar la estructura para realizar ejecutar el REQUEST
	strcpy(cSend.cMethod,"GET");
	strcpy(cSend.cURL, url); 
	strcpy(cSend.cFrmData,sData);
	strcpy(cSend.cObjectNameURI, uri); 
	strcpy(cSend.cHeaders,"Content-Type: text/xml; encoding=utf-8");
	strcpy(cSend.cIsSSL,ssl);    
	cSend.PortNumber=atoi(port); 
	cSend.cFrmDataLength=sData.GetLength();
	
	//HACER EL REQUEST HTTP
	iRet= MakeHTTPRequestONE(&cSend,&cReturn);
	
	//Validar respuesta
	if(iRet != -1)
	{
		//Parsear respuesta
		strcpy(respuesta, cReturn.Respuesta);
		
		//Pasar a mayusculas
		MinToMay(respuesta);

		sRespuesta = respuesta;
	}

	return sRespuesta;
}

//Funcion para pasar de minusculas a MAYUSCULAS
void MinToMay(char string[])
{
	int i = 0;
	int desp = 'a'-'A';
	for (i=0;string[i]!='\0';++i)
	{
		if(string[i] >= 'a'&&string[i]<='z')
		{
			string[i] = string[i]-desp;
		}
	}
}
