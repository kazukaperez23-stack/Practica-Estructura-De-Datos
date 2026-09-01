# Ordenamiento de un conjunto grande de datos en C++

Comparación experimental de tres estrategias de ordenamiento sobre **100.000 palabras**,
todas implementadas desde cero: QuickSort sobre un arreglo dinámico, HeapSort sobre un
heap binario, y un árbol AVL del que se extrae el orden con un recorrido inorden.

> Práctica I — ST0245 Estructuras de Datos y Algoritmos · Universidad EAFIT

No se utiliza ninguna función de ordenamiento de la biblioteca estándar: no hay
`std::sort`, `std::stable_sort`, `std::priority_queue`, `std::make_heap`, `std::map`
ni `std::set` en el código de ordenamiento.

## Resultados

Tiempos en milisegundos, promedio de 3 corridas, compilación `-O2`:

| Escenario | QuickSort | HeapSort | Árbol AVL |
|---|---|---|---|
| Aleatorio | **19,61** | 45,72 | 42,03 |
| Ordenado | **7,24** | 34,40 | 18,64 |
| Inverso | **13,77** | 32,59 | 18,16 |

### Costo normalizado

Dividir el tiempo entre `n · log₂(n)` cancela el crecimiento teórico: si el cociente
se mantiene constante, la implementación se comporta realmente como O(n log n).

![Costo normalizado](charts/chart_normalized.png)

| n | QuickSort | HeapSort | AVL |
|---|---|---|---|
| 1.000 | 11,69 | 19,50 | 12,39 |
| 100.000 | 12,01 | 25,49 | 22,62 |
| **Deriva** | **+2,7 %** | +31 % | +83 % |

QuickSort se mantiene plano a lo largo de dos órdenes de magnitud. La deriva
ascendente de las otras dos es el factor constante que la notación Big O no captura:
fallos de caché y, en el AVL, 100.000 asignaciones dinámicas individuales.

## Hallazgos

**QuickSort es más rápido con datos ya ordenados** (7,24 ms contra 19,61 ms), justo en
el escenario que se supone es su peor caso. La mediana de tres escoge la mediana real
cuando el arreglo está ordenado, así que cada partición queda perfectamente balanceada.
Es la evidencia empírica de que el caso O(n²) fue evitado por diseño.

**El árbol AVL es más lento con datos aleatorios** (42,03 ms contra 18,64 ms). Las
rotaciones impiden que degenere en cualquier caso, así que lo que decide el tiempo es
la localidad de memoria: con entrada ordenada cada inserción reutiliza una rama que
permanece en caché; con entrada aleatoria cada inserción recorre un camino distinto
entre 100.000 nodos dispersos.

**HeapSort es el menos sensible al orden de la entrada** (40 % de variación frente al
171 % de QuickSort). Construye el heap y extrae `n` máximos, y ese trabajo es el mismo
sin importar cómo lleguen los datos. Por eso su mejor caso, promedio y peor caso
coinciden en O(n log n).

## Complejidad y memoria

| Algoritmo | Mejor | Promedio | Peor | Espacio extra |
|---|---|---|---|---|
| QuickSort | O(n log n) | O(n log n) | O(n²) * | O(log n) |
| HeapSort | O(n log n) | O(n log n) | O(n log n) | O(1) |
| Árbol AVL | O(n log n) | O(n log n) | O(n log n) | O(n) |

\* Mitigado con mediana de tres y corte a insertion sort; el experimento confirma que
no se activa.

Estimación con `sizeof` para n = 100.000 (943.878 caracteres):

| Estructura | Por elemento | Total aproximado |
|---|---|---|
| `vector<string>` | 32 B | 3,95 MB |
| Árbol AVL | 56 B (string + 2 punteros + altura) | 6,24 MB |

El árbol consume 2,29 MB más — un 58 % — por los mismos datos. Con las 100.000
palabras alcanza una altura de **20**; el mínimo teórico es 17 y la cota de un AVL
es ≈ 24.

## Compilar y ejecutar

Requiere un compilador C++17 y CMake.

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
./sort_practice dataset.txt
```

En CLion basta con abrir la carpeta: detecta el `CMakeLists.txt` automáticamente.
**Usar el perfil Release** — una compilación Debug (`-O0`) corre entre 4 y 8 veces más
lenta y no reproduce estos números. El `CMakeLists.txt` emite una advertencia si se
configura en Debug.

El programa verifica que las tres estrategias produzcan resultados idénticos, estima
la memoria, ejecuta los dos experimentos y escribe `benchmark_results.csv` y
`sorted_dataset.txt`. Devuelve 0 solo si las cuatro verificaciones pasan.

## Estructura

```
src/
  main.cpp              Programa principal: verificación, memoria y los dos experimentos
  QuickSort.hpp         QuickSort con mediana de tres y corte a insertion sort
  HeapSort.hpp          Clase BinaryHeap (TAD completo) y el HeapSort construido sobre ella
  AVLTree.hpp           Árbol AVL con rotaciones y recorrido inorden
  Benchmark.hpp         Banco de pruebas: escenarios, repeticiones y escalabilidad
  Utils.hpp             Carga del dataset y utilidades
  generate_dataset.cpp  Genera dataset.txt barajando words_alpha.txt

dataset.txt             Entrada: 100.000 palabras en inglés, sin duplicados
sorted_dataset.txt      Salida: las mismas palabras ordenadas
benchmark_results.csv   Todas las mediciones
charts/                 Gráficas del informe
Report.docx             Informe completo
```

## Metodología

- Cada valor es la media de 3 corridas consecutivas.
- La entrada se copia antes de cada corrida, de modo que el costo de la copia queda
  fuera de la región cronometrada.
- Las tres estrategias reciben exactamente la misma entrada en cada escenario.
- Las entradas ordenada e invertida se generan con el QuickSort propio, nunca con un
  ordenamiento de biblioteca.
- En el AVL se cronometra la estrategia completa: construcción del árbol más el
  recorrido inorden.
