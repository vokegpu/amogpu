# Amo GPU

Entenda, a GPU é muito importante para nós programdores e programadoras, sem ela aplicações que nós usamos não teria o minimo de performance e design bonito!
Ao longo dos anos sempre amei qualquer coisa que envolvia assuntos visuais que usam das APIs OpenGL, DirectX e Vulkan! Infelizmente poucas pessoas se interessam por este íncrivel lado do hardware!

Esse repo é um simples projeto mostrando como podemos utilizar corretamente funções do OpenGL moderno, diferente da versão legacy, lembre é preciso estudar um pouco o código antes de querer aplicar por aí em qualquer contexto GL (OpenGL ES, WebGL etc)!

--

- O que é tessellator?
Do mesmo modo que traçamos linhas para formar um tecido, em computação trassamos linhas por vértices, quando tratamos de elementos UI que elaboram uma GUI, é preciso manipular cada evento de cada elemento UI sincronizado com o desenho, para isso preciamos passar para a GPU vértices e as posições na tela, entretanto não dá pra só atualizar a todo tick e passar vértices a todo tick e a todo momento.

- O que é dynamic batching?
Batch é salvar em forma de lote e utilizar depois, diferente de você enviar vértices todo instante (tick), podemos armazenar as vértices em lote e depois renderizar, e mais, salvar as posições, cor ou textura para renderizar sem a necessidade de mandar todas as vértices denovo para a GPU, com isso é possível ter uma performance muito maior se comparada com outros métodos.

--

O funcionamento é simples:
```c++
dynamic_batching batch;

batch.invoke(); // Chamamos a GPU.
batch.revoke(); // Finalizamos o segmento de desenhar da GPU.

// Em um loop com contexto OpenGL.
batch.draw();
```
