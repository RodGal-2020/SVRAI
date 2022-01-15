;;; Tema 3, Ejercicio 3: Implementa Distributed Breakout y aplícalo a diversos CSPs.
;;; Autor: José Antonio Rodríguez Gallego
;;; Referencia: https://www.cs.us.es/~fsancho/Cursos/SVRAI/DisCSP.md.html#breakoutdistribuido
;;; Última modificación: 15 / 01 / 2022, 10:55

globals [
  domain ; El dominio de las variables
]
breed [nodes node]
nodes-own [
  value ; El valor que toma el nodo-variable
  mode ; El modo hace referencia a los modos de espera ok? e improve del algoritmo breakout
  received-messages ; Los mensajes recibidos son una lista de listas de mensajes de otros agentes [mensaje1, mensaje2...], con mensaje1 = ["ok?" x_j d_j]
  agent-view ; La vista del agente, una lista de elementos [x_j, d_j]
  better-value ; El mejor valor que podría tomar el agente
  better-value-improvement ; La mejora que supondría tomar el mejor valor
]

undirected-link-breed [constraints constraint]
constraints-own [
  weight ; Los pesos asociados a las restricciones
]

to load
  ;; FUTURO: Lo ideal sería que leyese ficheros, determinando el valor de n-nodes y las restricciones en cuestión
  ;; FUTURO: Podríamos mejorar esto incluyendo un procedimiento de lectura de ficheros
  set domain range n-colors ;; FUTURO: El dominio normalmente dependería de cada nodo
end

to setup
  clear-all

  load

  setup-nodes
  setup-constraints

  layout-circle sort nodes 13
end

to setup-nodes
  set-default-shape nodes "circle"
  create-nodes n-nodes
end

to setup-constraints
  ;; FUTURO: Modificar para generalizar
  ask nodes with [not any? link-neighbors and who != n-nodes - 1] [
    ; Garantizamos la conexión del grafo
    create-constraint-with node (who + 1) ; Creamos conexión con el siguiente nodo
  ]
    ask nodes [
    ifelse (fixed-constraints) [
       repeat common-degree [
        create-constraint-with one-of other nodes
     ]
    ] [
      repeat n-nodes - 1 [
        if (random-float 1 < constraint-probability) [
        create-constraint-with one-of other nodes
      ]
      ]
    ]
  ]
end

to setup-distributed-breakout
  ask nodes [
    set value one-of domain
    update-labels
    set mode "ok?"
    set received-messages []
  ]

  ; Ponemos lo siguiente aparte al necesitar lo anterior para arrancar el algoritmo
  ask nodes [
    ; Que todos envíen mensajes ok? a sus vecinos para preparar el inicio del algoritmo
    let message (list "ok?" who value) ; ("ok?", x_j, d_j)
    ask link-neighbors [
      if (not member? message received-messages) [
        set received-messages lput message received-messages ; ; Si aún no ha recibido este mensaje añadimos [ok x_j d_j] a la lista de mensajes recibidos
      ]
    ]
  ]

  ask nodes [ ; Inicializamos la visión del nodo: su valor junto con todos los recibidos
    set agent-view (list (list who value))
    foreach (map [message -> list (item 2 message) (item 1 message)] received-messages) [x -> set agent-view lput x agent-view]
  ]

  ask constraints [
    set weight 1
  ]

  ask constraints [
    set-thickness
  ]

  reset-ticks
end

to go-distributed-breakout
  ask nodes [
    ;; Modo wait_ok?
    ifelse mode = "ok?" [
    ;; Si se recibe ("Ok?", x_j, d_j) hacer:
    let ok-messages filter [message -> first message = "ok?"] received-messages ; Tripletes cuya primera componente sea "ok?"
    set agent-view ; La añadimos a agent-view...
      sentence (map [
    ;; añadir (x_j, d_j) a agent_view
        message -> lput item 2 message (lput item 1 message []) ; Incluimos x_j, d_j, del mensaje, para cada mensaje de entre los mensajes ok?
      ] ok-messages) agent-view

    update-list ; Colapsamos received-messages y agent-view

    ;; Si se han recibido mensajes "Ok?" de todos los vecinos, hacer:
    let ok-message-senders map [message -> item 1 message] ok-messages ; Vemos quién mandó mensajes "ok?"
    if (sort [who] of link-neighbors = sort ok-message-senders) [ ; Si recibió mensajes de todos los vecinos
        ; Comprobación: ask nodes [show sort [who] of link-neighbors = sort map [message -> item 1 message] filter [message -> first message = "ok?"] received-messages]
    ;; enviar_improve
      send-improve
    ;; entrar en Modo wait_improve
      set mode "improve"
    ] ; Si no recibió mensajes ok? de todos los vecinos seguimos en mode = "ok?", esto es, esperando más mensajes "ok?"

    ] [

    ;; Modo wait_improve
    ;; si se recibe ("improve", x_j, mejora, eval) hacer:

    update-list ; Colapsamos received-messages

 		;; grabar el mensaje ;; Incluido en send-improve, no es más que añadirlo a received-messages tan pronto se intenta lanzar, cuestión que no representa un comportamiento completamente real al ser instantáneo.

 		; Si se han recibido mensajes improve de todos los vecinos, hacer:
    let improve-messages filter [message -> first message = "improve"] received-messages
    let improve-message-senders map [message -> item 1 message] improve-messages ; Vemos quién mandó mensajes "improve"
    if (sort [who] of link-neighbors = sort improve-message-senders) [
    ; Prueba: ask nodes [show sort [who] of link-neighbors = sort map [message -> item 1 message] filter [message -> first message = "improve"] received-messages]
        ;	enviar_ok
        send-ok
        ;	limpiar agent_view
        set agent-view []
 		    ;	entrar en Modo wait_ok
        set mode "ok?"
 		
      ] ; Si no se han recibido mensajes improve de todos los vecinos
        ;entrar en Modo wait_improve
        ; set mode "improve" ; Esto realmente no haría falta, pues ya se encuentra en el modo improve
 	]

  ask constraints [
    set-thickness ; Actualizamos el grosor para que represente al peso asociado
  ]
]
  tick
  plot violated-constraints
  if (violated-constraints = 0) [
    show (word "Solución encontrada en " ticks " pasos")
    save-data true
    stop
  ]
  if (ticks > max-steps) [
    show (word "Perdemos la esperanza y detenemoos el algoritmo. Puede que no tenga solución. Quizás deberíamos lanzar una búsqueda de subgrafos completos con n-colors nodos")
    save-data false
    stop
  ]
end

to save-data[worked?]
  ;;; FUTURO: Guardar los datos para ejecutar el aprendizaje bayesiano y/o el análisis estadístico de los resultados
  ifelse (worked?) [
    show "Guardaríamos los datos del experimento exitoso para futuros estudios. No implementado aún"
  ] [
    show "Guardaríamos los datos del experimento fallido para futuros estudios. No implementado aún"
  ]

end

to send-ok
  ;; Si su mejora es la mayor de los vecinos, hacer:
  let improve-messages filter [msg -> first msg = "improve"] received-messages
  if (better-value-improvement > max map [x -> item 2 x] improve-messages) [
    ;; valor_actual ← nuevo_valor
    set value better-value
    update-labels
  ]

  ; Vemos si es un cuasimínimo
  let available-change violated-constraints < max (map [x -> item 2 x] improve-messages)
     ; Definición cuasimínimo: "está violando alguna restricción y ni él ni ninguno de sus vecinos puede hacer un cambio que resulte en un menor costo total para el sistema"
  ;; Si es un cuasi-mínimo local, hacer:
  if (violated-constraints > 0 and not available-change) [ ; Caso cuasimínimo
    ;; incrementa el peso de las restricciones violadas
    ask constraints with [[value] of end1 = [value] of end2] [
      if (next-weight = "+1") [
        set weight weight + 1
      ]
      if (next-weight = "+grado1+grado2") [
        set weight weight + count [link-neighbors] of end1 + count [link-neighbors] of end2
      ]

    ]
  ]
  ;; enviar ("Ok?", x_i, valor_actual) a los vecinos
    let message (list "ok?" who value)
    ask link-neighbors [set received-messages lput message received-messages]

end

to send-improve
  ;; eval_actual ← evaluar valor de valor_actual
  let eval-now violated-constraints

 	;; mi_mejora ← posible máxima mejora
  ;; nuevo_valor ← valor que da la máxima mejora
  let old-value value ; No lo actualizamos en este paso

  set better-value 0 ; Por empezar con algunos
  set better-value-improvement eval-now
  ;;; PENDIENTE: Confirmar que he calculado esto bien
  foreach domain [ x ->
    set value x ; Tomamos dicho valor para calcularlo de nuevo
    if (violated-constraints < better-value-improvement) [
      set better-value x
      set better-value-improvement violated-constraints
    ]
  ]

  ;;; DUDA: Entiendo que en este punto no actualiza, sino que solo se actualiza en el send-ok
  ;set value better-value
  ;set label word value "      "
  ;set color item value base-colors
  set value old-value ; Recuperamos el valor anterior
  ;; enviar("improve", x_i, mi_mejora, eval_actual) a vecinos
  let new-message (list "improve" who better-value eval-now)
  ; set received-messages lput new-message received-messages ;; Suponemos que no se incluye a sí mismo
  ask link-neighbors [
    set received-messages lput new-message received-messages
  ]

end

to update-list
  ; Colapsamos los mensajes recibidos, de forma tal q solo se tenga en cuenta el último recibido de cada agente
  let unseen-msg received-messages ; Guardamos ahí los mensajes que aún no hayamos repasado
  let l-msg [] ; ["ok?" x_j d_j]
  while [not empty? unseen-msg] [ ; Pararemos al visitarlos todos
      set l-msg last unseen-msg ; ["ok?" x_j d_j]
      set received-messages filter [message -> not (item 0 message = item 0 l-msg and item 1 message = item 1 l-msg)] received-messages ; Eliminamos todas las apariciones de mensajes de x_j del mismo tipo
      set received-messages lput l-msg received-messages ; y añadimos la última, guardada en l-msg
      set unseen-msg filter [message -> not (item 0 message = item 0 l-msg and item 1 message = item 1 l-msg)] unseen-msg ; Eliminamos los que acabamos de ver, esto es, nos quedamos con los diferentes
  ]

  ; Y lo mismo prácticamente para agent-view
  set unseen-msg agent-view
  set l-msg []
  while [not empty? unseen-msg] [
    set l-msg last unseen-msg ; [x_j d_j]
    set agent-view filter [message -> not (item 0 message = item 0 l-msg)] agent-view
    set agent-view lput l-msg agent-view
    set unseen-msg filter [message -> not (item 0 message = item 0 l-msg)] unseen-msg
  ]
end


;;; Visualización
to set-thickness
  set thickness 0.4 * (weight / total-weight)
end

to update-labels
  set label (word "x_" who "=" value "      ")
    set color item value base-colors
end

to-report violated-constraints
  ; Para el caso del problema de la coloración ;; FUTURO: Personalizar con load
  let exit 0
  ask constraints with [[value] of end1 = [value] of end2] [
    set exit exit + weight
  ]
  report exit
end

to-report r-n-constraints
  report count constraints
end

to-report total-weight
  report sum [weight] of constraints
end
@#$#@#$#@
GRAPHICS-WINDOW
210
10
647
448
-1
-1
13.0
1
10
1
1
1
0
0
0
1
-16
16
-16
16
0
0
1
ticks
30.0

BUTTON
210
452
423
502
setup
setup\nsetup-distributed-breakout
NIL
1
T
OBSERVER
NIL
NIL
NIL
NIL
1

MONITOR
657
293
780
338
NIL
violated-constraints
17
1
11

MONITOR
788
294
912
339
n-constraints
r-n-constraints
17
1
11

BUTTON
428
452
647
503
go
go-distributed-breakout
T
1
T
OBSERVER
NIL
NIL
NIL
NIL
1

CHOOSER
8
356
206
401
next-weight
next-weight
"+1" "+grado1+grado2"
1

SLIDER
8
267
204
300
constraint-probability
constraint-probability
0
1
0.3
0.1
1
NIL
HORIZONTAL

SLIDER
7
68
205
101
n-nodes
n-nodes
2
15
7.0
1
1
NIL
HORIZONTAL

SLIDER
8
230
204
263
common-degree
common-degree
0
100
2.0
1
1
NIL
HORIZONTAL

SWITCH
9
193
204
226
fixed-constraints
fixed-constraints
0
1
-1000

SLIDER
8
106
203
139
n-colors
n-colors
1
10
4.0
1
1
NIL
HORIZONTAL

PLOT
657
11
1049
288
violated-constraints
NIL
NIL
0.0
10.0
0.0
10.0
true
false
"" ""
PENS
"default" 1.0 0 -16777216 true "" "plot count turtles"

BUTTON
8
10
203
64
Parámetros
NIL
NIL
1
T
OBSERVER
NIL
NIL
NIL
NIL
1

BUTTON
9
145
204
189
Creación del grafo
NIL
NIL
1
T
OBSERVER
NIL
NIL
NIL
NIL
1

BUTTON
7
308
205
351
Evolución del algoritmo
NIL
NIL
1
T
OBSERVER
NIL
NIL
NIL
NIL
1

TEXTBOX
660
349
1239
637
Rápida explicación de los parámetros:\n* n-nodes: Número de nodos\n* n-colors: Colores disponibles\n* fixed-constraints: Número fijo (aprox) de restriccioness o dependiente de constraint-probability?\n* common-degree: Grado esperado de cada nodo si fixed-constraints. \n* constraint-probability: Probabilidad de incluir una nueva restricción al contruir el grafo.\n* next-weight: Evolución de los pesos durante el algoritmo al encontrar un cuasimínimo. Con +1 se incrementa el valor en 1, y con +grado1+grado2 se incrementa en el valor de la suma de los grados de las variables involucradas.\n* max-steps: Número máximo de pasos antes de detener el algoritmo
15
0.0
0

INPUTBOX
6
406
207
466
max-steps
2000.0
1
0
Number

@#$#@#$#@
## WHAT IS IT?

(a general understanding of what the model is trying to show or explain)

## HOW IT WORKS

(what rules the agents use to create the overall behavior of the model)

## HOW TO USE IT

(how to use the model, including a description of each of the items in the Interface tab)

## THINGS TO NOTICE

(suggested things for the user to notice while running the model)

## THINGS TO TRY

(suggested things for the user to try to do (move sliders, switches, etc.) with the model)

## EXTENDING THE MODEL

(suggested things to add or change in the Code tab to make the model more complicated, detailed, accurate, etc.)

## NETLOGO FEATURES

(interesting or unusual features of NetLogo that the model uses, particularly in the Code tab; or where workarounds were needed for missing features)

## RELATED MODELS

(models in the NetLogo Models Library and elsewhere which are of related interest)

## CREDITS AND REFERENCES

(a reference to the model's URL on the web if it has one, as well as any other necessary credits, citations, and links)
@#$#@#$#@
default
true
0
Polygon -7500403 true true 150 5 40 250 150 205 260 250

airplane
true
0
Polygon -7500403 true true 150 0 135 15 120 60 120 105 15 165 15 195 120 180 135 240 105 270 120 285 150 270 180 285 210 270 165 240 180 180 285 195 285 165 180 105 180 60 165 15

arrow
true
0
Polygon -7500403 true true 150 0 0 150 105 150 105 293 195 293 195 150 300 150

box
false
0
Polygon -7500403 true true 150 285 285 225 285 75 150 135
Polygon -7500403 true true 150 135 15 75 150 15 285 75
Polygon -7500403 true true 15 75 15 225 150 285 150 135
Line -16777216 false 150 285 150 135
Line -16777216 false 150 135 15 75
Line -16777216 false 150 135 285 75

bug
true
0
Circle -7500403 true true 96 182 108
Circle -7500403 true true 110 127 80
Circle -7500403 true true 110 75 80
Line -7500403 true 150 100 80 30
Line -7500403 true 150 100 220 30

butterfly
true
0
Polygon -7500403 true true 150 165 209 199 225 225 225 255 195 270 165 255 150 240
Polygon -7500403 true true 150 165 89 198 75 225 75 255 105 270 135 255 150 240
Polygon -7500403 true true 139 148 100 105 55 90 25 90 10 105 10 135 25 180 40 195 85 194 139 163
Polygon -7500403 true true 162 150 200 105 245 90 275 90 290 105 290 135 275 180 260 195 215 195 162 165
Polygon -16777216 true false 150 255 135 225 120 150 135 120 150 105 165 120 180 150 165 225
Circle -16777216 true false 135 90 30
Line -16777216 false 150 105 195 60
Line -16777216 false 150 105 105 60

car
false
0
Polygon -7500403 true true 300 180 279 164 261 144 240 135 226 132 213 106 203 84 185 63 159 50 135 50 75 60 0 150 0 165 0 225 300 225 300 180
Circle -16777216 true false 180 180 90
Circle -16777216 true false 30 180 90
Polygon -16777216 true false 162 80 132 78 134 135 209 135 194 105 189 96 180 89
Circle -7500403 true true 47 195 58
Circle -7500403 true true 195 195 58

circle
false
0
Circle -7500403 true true 0 0 300

circle 2
false
0
Circle -7500403 true true 0 0 300
Circle -16777216 true false 30 30 240

cow
false
0
Polygon -7500403 true true 200 193 197 249 179 249 177 196 166 187 140 189 93 191 78 179 72 211 49 209 48 181 37 149 25 120 25 89 45 72 103 84 179 75 198 76 252 64 272 81 293 103 285 121 255 121 242 118 224 167
Polygon -7500403 true true 73 210 86 251 62 249 48 208
Polygon -7500403 true true 25 114 16 195 9 204 23 213 25 200 39 123

cylinder
false
0
Circle -7500403 true true 0 0 300

dot
false
0
Circle -7500403 true true 90 90 120

face happy
false
0
Circle -7500403 true true 8 8 285
Circle -16777216 true false 60 75 60
Circle -16777216 true false 180 75 60
Polygon -16777216 true false 150 255 90 239 62 213 47 191 67 179 90 203 109 218 150 225 192 218 210 203 227 181 251 194 236 217 212 240

face neutral
false
0
Circle -7500403 true true 8 7 285
Circle -16777216 true false 60 75 60
Circle -16777216 true false 180 75 60
Rectangle -16777216 true false 60 195 240 225

face sad
false
0
Circle -7500403 true true 8 8 285
Circle -16777216 true false 60 75 60
Circle -16777216 true false 180 75 60
Polygon -16777216 true false 150 168 90 184 62 210 47 232 67 244 90 220 109 205 150 198 192 205 210 220 227 242 251 229 236 206 212 183

fish
false
0
Polygon -1 true false 44 131 21 87 15 86 0 120 15 150 0 180 13 214 20 212 45 166
Polygon -1 true false 135 195 119 235 95 218 76 210 46 204 60 165
Polygon -1 true false 75 45 83 77 71 103 86 114 166 78 135 60
Polygon -7500403 true true 30 136 151 77 226 81 280 119 292 146 292 160 287 170 270 195 195 210 151 212 30 166
Circle -16777216 true false 215 106 30

flag
false
0
Rectangle -7500403 true true 60 15 75 300
Polygon -7500403 true true 90 150 270 90 90 30
Line -7500403 true 75 135 90 135
Line -7500403 true 75 45 90 45

flower
false
0
Polygon -10899396 true false 135 120 165 165 180 210 180 240 150 300 165 300 195 240 195 195 165 135
Circle -7500403 true true 85 132 38
Circle -7500403 true true 130 147 38
Circle -7500403 true true 192 85 38
Circle -7500403 true true 85 40 38
Circle -7500403 true true 177 40 38
Circle -7500403 true true 177 132 38
Circle -7500403 true true 70 85 38
Circle -7500403 true true 130 25 38
Circle -7500403 true true 96 51 108
Circle -16777216 true false 113 68 74
Polygon -10899396 true false 189 233 219 188 249 173 279 188 234 218
Polygon -10899396 true false 180 255 150 210 105 210 75 240 135 240

house
false
0
Rectangle -7500403 true true 45 120 255 285
Rectangle -16777216 true false 120 210 180 285
Polygon -7500403 true true 15 120 150 15 285 120
Line -16777216 false 30 120 270 120

leaf
false
0
Polygon -7500403 true true 150 210 135 195 120 210 60 210 30 195 60 180 60 165 15 135 30 120 15 105 40 104 45 90 60 90 90 105 105 120 120 120 105 60 120 60 135 30 150 15 165 30 180 60 195 60 180 120 195 120 210 105 240 90 255 90 263 104 285 105 270 120 285 135 240 165 240 180 270 195 240 210 180 210 165 195
Polygon -7500403 true true 135 195 135 240 120 255 105 255 105 285 135 285 165 240 165 195

line
true
0
Line -7500403 true 150 0 150 300

line half
true
0
Line -7500403 true 150 0 150 150

pentagon
false
0
Polygon -7500403 true true 150 15 15 120 60 285 240 285 285 120

person
false
0
Circle -7500403 true true 110 5 80
Polygon -7500403 true true 105 90 120 195 90 285 105 300 135 300 150 225 165 300 195 300 210 285 180 195 195 90
Rectangle -7500403 true true 127 79 172 94
Polygon -7500403 true true 195 90 240 150 225 180 165 105
Polygon -7500403 true true 105 90 60 150 75 180 135 105

plant
false
0
Rectangle -7500403 true true 135 90 165 300
Polygon -7500403 true true 135 255 90 210 45 195 75 255 135 285
Polygon -7500403 true true 165 255 210 210 255 195 225 255 165 285
Polygon -7500403 true true 135 180 90 135 45 120 75 180 135 210
Polygon -7500403 true true 165 180 165 210 225 180 255 120 210 135
Polygon -7500403 true true 135 105 90 60 45 45 75 105 135 135
Polygon -7500403 true true 165 105 165 135 225 105 255 45 210 60
Polygon -7500403 true true 135 90 120 45 150 15 180 45 165 90

sheep
false
15
Circle -1 true true 203 65 88
Circle -1 true true 70 65 162
Circle -1 true true 150 105 120
Polygon -7500403 true false 218 120 240 165 255 165 278 120
Circle -7500403 true false 214 72 67
Rectangle -1 true true 164 223 179 298
Polygon -1 true true 45 285 30 285 30 240 15 195 45 210
Circle -1 true true 3 83 150
Rectangle -1 true true 65 221 80 296
Polygon -1 true true 195 285 210 285 210 240 240 210 195 210
Polygon -7500403 true false 276 85 285 105 302 99 294 83
Polygon -7500403 true false 219 85 210 105 193 99 201 83

square
false
0
Rectangle -7500403 true true 30 30 270 270

square 2
false
0
Rectangle -7500403 true true 30 30 270 270
Rectangle -16777216 true false 60 60 240 240

star
false
0
Polygon -7500403 true true 151 1 185 108 298 108 207 175 242 282 151 216 59 282 94 175 3 108 116 108

target
false
0
Circle -7500403 true true 0 0 300
Circle -16777216 true false 30 30 240
Circle -7500403 true true 60 60 180
Circle -16777216 true false 90 90 120
Circle -7500403 true true 120 120 60

tree
false
0
Circle -7500403 true true 118 3 94
Rectangle -6459832 true false 120 195 180 300
Circle -7500403 true true 65 21 108
Circle -7500403 true true 116 41 127
Circle -7500403 true true 45 90 120
Circle -7500403 true true 104 74 152

triangle
false
0
Polygon -7500403 true true 150 30 15 255 285 255

triangle 2
false
0
Polygon -7500403 true true 150 30 15 255 285 255
Polygon -16777216 true false 151 99 225 223 75 224

truck
false
0
Rectangle -7500403 true true 4 45 195 187
Polygon -7500403 true true 296 193 296 150 259 134 244 104 208 104 207 194
Rectangle -1 true false 195 60 195 105
Polygon -16777216 true false 238 112 252 141 219 141 218 112
Circle -16777216 true false 234 174 42
Rectangle -7500403 true true 181 185 214 194
Circle -16777216 true false 144 174 42
Circle -16777216 true false 24 174 42
Circle -7500403 false true 24 174 42
Circle -7500403 false true 144 174 42
Circle -7500403 false true 234 174 42

turtle
true
0
Polygon -10899396 true false 215 204 240 233 246 254 228 266 215 252 193 210
Polygon -10899396 true false 195 90 225 75 245 75 260 89 269 108 261 124 240 105 225 105 210 105
Polygon -10899396 true false 105 90 75 75 55 75 40 89 31 108 39 124 60 105 75 105 90 105
Polygon -10899396 true false 132 85 134 64 107 51 108 17 150 2 192 18 192 52 169 65 172 87
Polygon -10899396 true false 85 204 60 233 54 254 72 266 85 252 107 210
Polygon -7500403 true true 119 75 179 75 209 101 224 135 220 225 175 261 128 261 81 224 74 135 88 99

wheel
false
0
Circle -7500403 true true 3 3 294
Circle -16777216 true false 30 30 240
Line -7500403 true 150 285 150 15
Line -7500403 true 15 150 285 150
Circle -7500403 true true 120 120 60
Line -7500403 true 216 40 79 269
Line -7500403 true 40 84 269 221
Line -7500403 true 40 216 269 79
Line -7500403 true 84 40 221 269

wolf
false
0
Polygon -16777216 true false 253 133 245 131 245 133
Polygon -7500403 true true 2 194 13 197 30 191 38 193 38 205 20 226 20 257 27 265 38 266 40 260 31 253 31 230 60 206 68 198 75 209 66 228 65 243 82 261 84 268 100 267 103 261 77 239 79 231 100 207 98 196 119 201 143 202 160 195 166 210 172 213 173 238 167 251 160 248 154 265 169 264 178 247 186 240 198 260 200 271 217 271 219 262 207 258 195 230 192 198 210 184 227 164 242 144 259 145 284 151 277 141 293 140 299 134 297 127 273 119 270 105
Polygon -7500403 true true -1 195 14 180 36 166 40 153 53 140 82 131 134 133 159 126 188 115 227 108 236 102 238 98 268 86 269 92 281 87 269 103 269 113

x
false
0
Polygon -7500403 true true 270 75 225 30 30 225 75 270
Polygon -7500403 true true 30 75 75 30 270 225 225 270
@#$#@#$#@
NetLogo 6.2.1
@#$#@#$#@
@#$#@#$#@
@#$#@#$#@
@#$#@#$#@
@#$#@#$#@
default
0.0
-0.2 0 0.0 1.0
0.0 1 1.0 0.0
0.2 0 0.0 1.0
link direction
true
0
Line -7500403 true 150 150 90 180
Line -7500403 true 150 150 210 180
@#$#@#$#@
0
@#$#@#$#@
