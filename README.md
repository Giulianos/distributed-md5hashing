# Introduccion
El objetivo del sistema es calcular de forma distribuida los hashes md5 de un grupo de archivos, recibidos por parámetro. Para esto se utilizaron diferentes procesos y mecanismos de comunicación: IPC’s (pipes, shared memory y semáforos). Por otra parte, los hashes md5 de los archivos procesados son almacenados en un .txt .
La estructura de los archivos que componen el sistema se divide en:
- #Application#
Este proceso es el encargado de comunicarse tanto con los esclavos como con la view. En cuanto a los esclavos, lo que hace es: crearlos, establecer los pipes de comunicación con cada uno de ellos y distribuir las tareas a realizar mandando de a JOBSIZE cantidades a cada esclavo que se encuentre libre. Por otra parte, cuando comienza la ejecución de este proceso, se muestra el PID para que mediante el comando ./view PID se pueda ver la salida de los archivos procesados en la pantalla. Creímos que era necesario parar el programa y permitirle al usuario optar por abrir la view o no, ya que el enunciado establece que la view podría aparecer o no.
- **View**
Es la parte visual del sistema. En este proceso se obtienen los datos guardados en la Shared Memory por el proceso aplicación y se imprimen de manera secuencial. Para esto, se recibe como parámetro del programa el PID del proceso aplicación del cual se quiere obtener su salida.
- **Slave**
Se ocupan de procesar los archivos y enviar mediante pipes a su padre los hashes md5. Para esto, optamos por reemplazar la entrada y salida estándar de estos procesos por un pipe generado por el proceso aplicación. Además, para calcular cada hash md5, forkeamos cada proceso esclavo y creamos un pipe en el cual se ejecuta el comando md5sum, redireccionando su salida a este pipe.
- **Queue**
Creamos este ADT para el manejo de colas de tareas requerido para poder realizar FIFO al momento de procesar los archivos. Internamente es una Linked List con header con referencias al primer y último elemento de ésta.

# Consideraciones
- Los archivos se distribuyen utilizando FIFO para enviarlos a los esclavos de a JOBSIZE cantidades.
- Se elige el idioma inglés por defecto (mensajes, variables, código en general).
- Se permite ejecutar múltiples vistas conectadas simultáneamente al mismo proceso aplicación. Para esto se debe ejecutar view varias veces con el mismo PID (el cual debe corresponder a ese proceso aplicación).
- Se utilizó un sistema de máquina de estados para facilitar el desarrollo y la lectura del código en donde se lee de memoria compartida.
- Se decidió poner un límite de espacio en memoria compartida de 10000 int (~40 KB). Esto permite procesar miles de archivos sin generar problemas de espacio de memoria.
- Se decidió que la cantidad de tareas a enviar y la cantidad de esclavos sea 3. Sin embargo se puede modificar esto cambiando las constantes definidas como JOBSIZE y WORKERS_QUANTITY en applicationProcess.h .
- A pesar de que sabemos que lo siguiente no cumple el segundo principio de la filosofía de Unix (ver bibliografía), se decidió que el proceso aplicación muestre información acerca de la asignación de tareas para poder hacer más evidente (de forma visual) el hecho de que los distintos esclavos tardan diferentes cantidades de tiempo en procesar los distintos archivos. Esto incluye la espera de que el usuario abra o no la view, anteriormente mencionada.

# Mejoras posibles
Por cuestiones de tiempo no pudimos implementar estas mejoras, pero creímos que era necesario aclarar que eran parte de nuestro objetivo.
- Mejor modularización del código: por ejemplo, mayor abstracción entre la capa de comunicación entre procesos y la lógica de la distribución de tareas. Pensábamos utilizar callbacks para esto, es decir, funciones que se encarguen de la comunicación entre procesos.
- Mejor manejo de errores.
- Mejor manejo de memoria (actualmente puede llegar a faltar algún free).

# Conclusiones
- Los semáforos son clave en la comunicación entre procesos para evitar condiciones de carrera, en donde el acceso a un recurso está dado por quién llegó primero, lo cual genera corrupción de los datos, en nuestro caso, la Shared Memory utilizada por los procesos aplicación y view.
- Se podría distribuir los archivos de manera más eficiente utilizando un algoritmo de planificación más complejo. Inicialmente se había implementado distribución por tamaño de archivos. Sin embargo se nos indicó que no era lo esperado ya que se quería que los esclavos tardaran diferentes tiempos en ejecutar los archivos designados, sin importar que esto degrade la performace.

# Códigos de referencia
- http://www.chuidiang.org/clinux/ipcs/semaforo.php
- http://www.chuidiang.org/clinux/ipcs/mem_comp.php
- Manual de Unix (comando man)

# Bibliografía
- Beej’s Guide to Unix IPC - http://beej.us/guide/bgipc/
- Unix philosophy - https://en.wikipedia.org/wiki/Unix_philosophy
- Unix Network Programming - Richard Stevens
