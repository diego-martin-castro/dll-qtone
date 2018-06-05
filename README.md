##INSTRUCCIONES DE USO:

1- En Project Settings del proyecto donde se lo vaya a incluir -> ir a la solapa: Link ->Object/Library modules: -> Agregar la librería QTONE.lib

2- En el .h de la clase donde se utilice la DLL incluir el .h QTONE.H

##FUNCIONES

**ObtenerContenidosONE(CString sIdContenido, CString sAmbiente)** 

Función para obenter un String FINAL con IDcategoria + NOMcategoria + IDdoc + DesDOC / separados por |#| (linea) y |&| Dato

**ObtenerDescripcionID(int empieza, CString sRespuesta, bool sDatos)** 

Función para obener el ID o la Descripción de los contenidos según el ID de area que se pase como parametro

**ObtenerDocumentosCodigo(int empieza, CString sRespuesta)** 

Función para obtener los codigos y documentos según la categoría que se pase como parametro

**ObtenerURLdescarga(CString sAmbiente)** 

Función para obtener la URL de descarga de los documentos

**ObtenerSeparador()** 

Función para obtener el separador por cada linea para su parseo

**ObtenerDivisor()** 

Función para obtener el divisor por cada data de una linea para su parseo