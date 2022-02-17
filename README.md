# Trabajos de SVRAI
Autor: José Antonio Rodríguez Gallego

Información detallada sobre el trabajo realizado se encuentra en la memoria, en formato HTML.

## Tema 2
En este tema me he centrado en la creación de un programa que aplique el algoritmo Value-Iteration, de una forma tan genérica como me ha sido posible. 

### Archivos principales
1 - Generador_MDPs.c 
</br>
  → Con este documento compilado somos capaces de generar multitud de matrices representando MDPs, con cierta personalización.

2 - Represesnta_MDP.c
</br>
→ Con este documento compilado somos capaces de generar multitud de matrices representando MDPs, con cierta personalización.

3 - Solver_MDPs.c
</br>
→ Con este documento compilado somos capaces de aplicar Value-Iteration ante múltiples escenarios: resolución de los elementos de la carpeta MDPS, resolución del problema de Marvin, resolución personalizable de otros problemas.

4 - Marvin el robot.nlogo
</br>
→ Simulación del comportamiento de Marvin el robot, del segundo ejercicio.

5 - Marvin el robot con Solver_MDPs
</br>
→ Guía para atacar el problema de Marvin con Solver_MDPs, indicando el lugar en el que se almacenan los resultados.

### Carpetas
#### MDP
Ejemplos de varias estructuras de datos necesarias para recurrir a las anteriores funcionalidades. Destacamos los elementos Marvin*.txt, necesarios para atacar el problema 2, el de Marvin el robot, con Value-Iteration. Se incluye también una referencia de formato.
#### MDPS
Conjunto de matrices asociadas a MDPs, generadas de forma aleatoria según Generador_MDPs.c.
#### stats
Resultados obtenidos a la hora de aplicar Value-Iteration a las matrices de MDPS, guardando variables de interés con la idea de poder llevar a cabo un análisis estadístico más adelante.
También existe un .txt utilizado para detectar errores a la hora de aplicar el algoritmo.

## Tema 3
En este tema me he centrado en la creación de un programa que aplique el algoritmo Breakout Distribuido, concretamente en el problema de la coloración de grafos.

### Archivos principales
Ej_Breakout_Distribuido.nlogo
</br>
→ El algoritmo ya indicado.

## Tema 7, 8.9
En este tema me he centrado en la creación de un programa que simule el dilema del prisionero iterado, para diferentes estrategias.

### Archivos principales
Prisionero_Iterado.nlogo
</br>
→ El procedimiento ya indicado.


