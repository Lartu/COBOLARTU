;Demo de cobolartu, por lartu (?

DATA:
$NOMBRE IS TEXT 32		;Nombre del jugador
$STRBUFFER IS TEXT 255	;String Buffer
$PAPA IS NUMBER			;Número a adivinar
$GUESS IS NUMBER		;Guess del jugador

PROCEDURE:
STORE "\WHITE*** JUEGO DE LA PAPA ***\NORMAL\n" IN $STRBUFFER
DISPLAY $STRBUFFER
DISPLAY "\RED¿Cuál es su nombre?\NORMAL "
ACCEPT $NOMBRE
JOIN "Bienvenido, " AND $NOMBRE IN $STRBUFFER
DISPLAY $STRBUFFER
DISPLAY "\nVamos a jugar el juego de la \CYANpapa\NORMAL. Hay que adivinar el número que\n"
DISPLAY "estoy pensando. ¿Listo? ¡Vamos!\n"
CALL @RANDOMIZE 1 100
; STORE $RANDOM IN $PAPA
STORE -1 IN $GUESS
WHILE $GUESS IS NOT EQUAL TO $PAPA THEN
	DISPLAY "Qué numero estoy pensando? "
	ACCEPT $GUESS
	IF $GUESS IS LOWER THAN $PAPA THEN
		DISPLAY "No, es menor!\n"
	END-IF
	IF $GUESS IS GREATER THAN $PAPA THEN
		DISPLAY "No, es mayor!\n"
	END-IF
REPEAT
DISPLAY "¡Bien, ganaste!\n"
END
