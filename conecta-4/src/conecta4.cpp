#include "conecta4.h"
#include <cmath>

Conecta4::Conecta4(const Tablero& tab, int met) {
  metrica_elegida = met;
  arbol_posibilidades = ArbolGeneral<Tablero>(tab);
  generar_arbol_posibilidades(arbol_posibilidades.raiz(), 0);
}

void Conecta4::generar_arbol_posibilidades(const ArbolGeneral<Tablero>::Nodo& raiz, int profundidad) {
  profundidad++; //-- Primero aumentamos la profundidad, pues acabamos de descender a un hijo (en caso de ser la raíz pasamos del nivel 0 al 1)
  if (profundidad > MAX_DEPTH) //-- Si ya estamos a la máxima profundidad no seguimos generando hijos
    return;
  
  int columnas = arbol_posibilidades.etiqueta(raiz).GetColumnas();
  //ArbolGeneral<Tablero>::Nodo hijo(raiz), hermano;  //-- No queremos perder el nodo hijo para poder insertar a su derecha
  ArbolGeneral<Tablero> aux;
  Tablero hijo(arbol_posibilidades.etiqueta(raiz));
  
  
  int i;
  bool insertado = false; //-- La forma de insertar se hace primero se inserta en el hijo izquierda y luego insertamos en los hermanos derecha, por lo tanto primero tenemos que buscar el primer hueco, si lo hay usamos insertar__hijomasizquierda(), pero para los siguientes hijosraiz tenemos que insertar como hermanos_derecha del hijo que ya tenemos, por eso usamos esta condicional para distinguir el tipo de inserción que tenemos que hacer
  
  for (i = 0; i < columnas && !insertado; ++i) {
    if (arbol_posibilidades.etiqueta(raiz).hayHueco(i) != -1) {
      hijo.cambiarTurno();
      insertado = hijo.colocarFicha(i);
      aux.AsignaRaiz(hijo);
      arbol_posibilidades.insertar_hijomasizquierda(raiz, aux);
      generar_arbol_posibilidades(arbol_posibilidades.hijomasizquierda(raiz), profundidad);
      
    }
  }

  //-- Para el resto de hijos
  ArbolGeneral<Tablero>::Nodo donde_insertar = arbol_posibilidades.hijomasizquierda(raiz);
  
  while (i < columnas) {
    if (arbol_posibilidades.etiqueta(raiz).hayHueco(i) != -1) {
      Tablero hermano(arbol_posibilidades.etiqueta(raiz)); 
      hermano.cambiarTurno();
      hermano.colocarFicha(i);
      aux.AsignaRaiz(hermano);
      arbol_posibilidades.insertar_hermanoderecha(donde_insertar, aux);
      donde_insertar = arbol_posibilidades.hermanoderecha(donde_insertar);
      generar_arbol_posibilidades(donde_insertar, profundidad);
    }
    i++;
  }
  return;
}

pair<ArbolGeneral<Tablero>::Nodo, int> Conecta4::recorrer_arbol(const ArbolGeneral<Tablero>::Nodo& raiz) {
  if(arbol_posibilidades.altura(raiz) == 0) {
    //cout << "Altura == 0" << endl;
    pair<ArbolGeneral<Tablero>::Nodo, int> p(raiz, metrica(arbol_posibilidades.etiqueta(raiz)));
    
    return p;
  }
  else {
    //..Resto de casos
    //..Para cada hijo llamo a recorrer_arbol 
    pair<ArbolGeneral<Tablero>::Nodo, int> maximo = recorrer_arbol(arbol_posibilidades.hijomasizquierda(raiz));
    maximo.second += metrica(arbol_posibilidades.etiqueta(raiz)) * arbol_posibilidades.altura(raiz);
    //cout << "maximo " << arbol_posibilidades.etiqueta(maximo.first) << maximo.second << endl;
    if(arbol_posibilidades.altura(raiz) != 1) {
      maximo.first = arbol_posibilidades.hijomasizquierda(raiz);
    }
    pair<ArbolGeneral<Tablero>::Nodo, int> intermedio(maximo);
    
    //cout << "Altura = " << arbol_posibilidades.altura(raiz) << ", hermanoderecha(intermedio.first)" << endl;
    //cout << arbol_posibilidades.etiqueta(arbol_posibilidades.hermanoderecha(intermedio.first));
    ArbolGeneral<Tablero>::Nodo  nodo_aux;
      
    while((arbol_posibilidades.hermanoderecha(intermedio.first)) != NULL) {
      nodo_aux = arbol_posibilidades.hermanoderecha(intermedio.first);
      
      intermedio = recorrer_arbol(nodo_aux); 
      intermedio.first =  nodo_aux;
      intermedio.second += metrica(arbol_posibilidades.etiqueta(nodo_aux)) * arbol_posibilidades.altura(nodo_aux);
      
      if(abs(maximo.second) < abs(intermedio.second))
	maximo = intermedio;
    }
    //cout << "Hermano derecha de: " << arbol_posibilidades.etiqueta(nodo_aux) << "NULL" << endl;
    
    return maximo;
  }
    
}

Tablero Conecta4::mejor_tablero(Tablero tablero) {
  if(metrica_elegida == 1) {
    return metrica1(tablero);
  }
  else {
    if(metrica_elegida == 2)
      return metrica2(tablero);
    pair<ArbolGeneral<Tablero>::Nodo, int> p;
    p = recorrer_arbol(arbol_posibilidades.raiz());
    return arbol_posibilidades.etiqueta(p.first);
  }
}

int Conecta4::metrica(Tablero &tablero) {
  //..Llamar a recorrer_arbol y quedarse con el tablero correspondiente
  
  int puntuacion;
  
  switch (metrica_elegida) {
    /* //..
  case 1:
    puntuacion = metrica1(tablero);
    break;
  case 2:
    puntuacion = metrica2(tablero);
    break;*/
  case 3:
    puntuacion = metrica_penultima(tablero);
    break;
  case 4:
    puntuacion = metrica_ultima(tablero);
    break;
  default:
    cout << "Error en selección de métrica para inserción." << endl;
    exit(-1);
    //..return puntuacion
  }

  return puntuacion;
} 

void Conecta4::actualizar(const Tablero& tablero) {
  arbol_posibilidades.AsignaRaiz(tablero);
  generar_arbol_posibilidades(arbol_posibilidades.raiz(), 0);
}

Tablero Conecta4::metrica1(Tablero &tablero) {
  int cols = tablero.GetColumnas();

  //Comprobamos si podemos ganar
  for(int i = 0; i < cols; ++i) {
    Tablero aux(tablero);
    if(aux.colocarFicha(i)) {
      if(aux.quienGana() == 2) {
	tablero.colocarFicha(i);
	return tablero;
      }
    }
  }

  //Cortamos jugadas ganadoras
  for(int i = 0; i < cols; ++i) {
    Tablero aux(tablero);
    aux.cambiarTurno();
    if(aux.colocarFicha(i)) {
      if(aux.quienGana() == 1) {
	tablero.colocarFicha(i);
	return tablero;
      }
    }
  }

  //Insertamos aleatoriamente
  bool insertada = false;
  do {
    int pos = rand() % cols;
    if(tablero.colocarFicha(pos))
      return tablero;
  } while(!insertada);
}

Tablero Conecta4::metrica2(Tablero &tablero) {
  int cols = tablero.GetColumnas();

  for(int i = 0; i < cols; ++i) {
    Tablero aux(tablero);
    aux.cambiarTurno();
    if(aux.colocarFicha(i)) {
      if(aux.quienGana() == 1) {
	tablero.colocarFicha(i);
	return tablero;
      }
    }
  }

  // Si el otro jugador en ningún caso hace un 4 en raya
  bool insertada = false;
  do {
    int pos = rand() % cols;
    if(tablero.colocarFicha(pos))
      return tablero;
  } while(!insertada);
}

int Conecta4::metrica_penultima(Tablero &tablero) { //..No es constante 
  if (tablero.quienGana() == 0)
    return metrica_ultima(tablero);

  if (tablero.quienGana() == 2)
    return 1000;

  else
    return -1500;
}


int Conecta4::metrica_ultima(const Tablero& tablero) { 
  //cout << tablero;
  int puntuacion = rand() % 100;
  //cout << "Puntuación: " << puntuacion << endl;
  return puntuacion;
}
