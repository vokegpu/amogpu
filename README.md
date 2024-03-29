# This library is deprecated and old.
# do not use

# Amogpu

The reason to this readme is written in another language and not english is because there is not good materials to learn OpenGL in portuguese (Brasil).
--

Entenda, a GPU é muito importante para nós programdores e programadoras, sem ela aplicações que nós usamos não teria o minimo de performance e design bonito!
infelizmente poucas pessoas se interessam por este incrível lado do hardware!
Essa biblioteca mostra como podemos utilizar corretamente funções do OpenGL moderno.

Tessellator, batch. shape builder e outros conceitos estão inseridos na amogpu, aqui está um simples resumo do que são:

- O que é tessellator?
Do mesmo modo que traçamos linhas para formar um tecido, em computação trassamos linhas por vértices, quando tratamos de elementos UI que elaboram uma GUI, é preciso manipular cada evento de cada elemento UI sincronizado com o desenho, para isso preciamos passar para a GPU vértices e as posições na tela, entretanto não dá pra só atualizar a todo tick e passar vértices a todo tick e a todo momento.

- O que é batch?
Batch é salvar em forma de lote e utilizar depois, diferente de você enviar vértices todo instante (tick), podemos armazenar as vértices em lote e depois renderizar, e mais, salvar as posições, cor ou textura para renderizar sem a necessidade de mandar todas as vértices denovo para a GPU, com isso é possível ter uma performance muito maior se comparada com outros métodos.

- O que é shape builder?
Se você quiser renderizar formas geométricas rapidamente, mas ela não é eficiênte para UI complexas, o nome builder por que ela constrói um shape e destrói logo em seguida.

---

# Get Start

Essa biblioteca foi compilado pra Windows-x86_x64, se você quiser contribuir com uma versão compilada em outro sistema fique a vontade.
Primeiramente baixe o repositório e copie a pasta `include/` & `lib/` dentro do MinGW, pronto! Agora você deve linkar:

`target_link_libraries(<projectname> amogpu)`
`g++ ... -lamogpu`

Inicialmente inclua `amogpu/amogpu` e chame a função `amogpu::init`, deste jeitinho ó:
```c++
#include <amogpu/amogpu.hpp>

// ...
amogpu::init(); // Não é pra ocorrer nenhum erro, caso sim reporte.

/**
 * Mainloop.
 **/
while (true) {
  // ...
  // Você deve chamar essa função apenas 1 vez antes de desenhar qualquer coisa.
  // Ela serve pra atualizar as matrizes de posição da camêra.
  amogpu::matrix();
  
  // ...
```

# Shape Builder

Shape builder é uma feature para desenhar circulos e retangulos de forma rápida e eficiente, mas ela não serve pra contextos aonde você precisa de performance e controle.
```c++
shape_builder shape;

/*
 * Main loop.
 */
while (true) {
   // Diferente de dynamic batch, você não precisa controlar invoke e revoke calls.
   shape.invoke();
   
   // Se você quiser criar um circulo use amogpu::shape::CIRCLE, se não use amogpu::shape::RECT
   shape.build(amogpu::shape::RECT, amogpu::vec4f(1.0f, 1.0f, 1.0f, 0.5f), 0); // o ultimo argumento é a texture id.
   shape.modal(0.0f, 0.0f, 1.0f, 1.0f); // se não tiver uma textura embutida no shape não é preciso chamar esse metódo!
   shape.draw(20, 20, 200, 200); // (x, y, width, height), se você estiver renderizando um circle então deixe width e height iguais.
   
   // Pronto é só isso.
   // Você pode chamar quantas vezes quiser.
   
   shape.build(amogpu::shape::RECT, amogpu::vec4f(1.0f, 1.0f, 1.0f, 1.0f)); 
   shape.draw(20, 60, 200, 200);
   
   shape.build(amogpu::shape::RECT, amogpu::vec4f(1.0f, 1.0f, 1.0f, 1.0f), texture_cool_cat_id);
   shape.modal(0.0f, 0.0f, 1.0f, 1.0f);
   shape.draw(0, 60, 200, 200);
   
   // Prontinho.
   shape.revoke();
}
```

A imagem a baixo é ilustrativa, se trata de outro código usando a feature shape_builder. 
![Alt text](splash/splash-shape-builder.png?raw=true)

# Dynamic Batching

O funcionamento é simples:
```c++
/**
 * Mainloop.
 **/
while (true) {  
  // ...
  // Qualquer lugar após glClear & glClearColor.
  batch1.draw();
  batch3.draw();
  batch2.draw();
  
  // ...
  // Final do loop.
}
```

Detalhes:
```c++
#include <amogpu/gpu_handler.hpp> // ou só #include <amogpu/amogpu.hpp>

// ...
dynamic_batching batch;

// Em 1 tick ou em poucos ticks aonde queremos mudar alguma coisa.
batch.invoke(); // Chamamos a GPU.
// Enviamos dados para a GPU.
batch.revoke(); // Finalizamos o segmento de desenhar da GPU.

// Em um loop com contexto OpenGL.
while (true) {
   batch.draw();
}
```

Dentro do segmento de desenhar podemos renderizar muitos rects (dá pra facilmente renderizar circulos mas nesse caso é feito no fragment shader):
```c++
batch.invoke(); // Iniciamos esse segmento.

x = 0;
y = 0;
w = 1.0f;
h = 1.0f;

batch.instance(20, 50); // Instancia contém 3 argumentos, 2 primeiros são [x, y] e o ultimo é o factor, mas não precisa especificar agora.
batch.fill(1.0f, 1.0f, 1.0f, 1.0f); // definimos a cor em RGBA normalisados (1.0 - 0.0).
batch.bind(f_renderer.texture_bitmap); // texture é um uint que guarda dados de uma textura.

// Como isso é um shape simples e não um grande desenho (e.g tile map, wireframe) você não precisa especificar um fator unico.
batch.modal(300, 300); // isso serve para definir o tamanho, mas isso só funciona pra shape simples.

// Mesh de vértices.
batch.vertex(x, y);
batch.vertex(x, y + h);
batch.vertex(x + w, y + h);
batch.vertex(x + w, y + h);
batch.vertex(x + w, y);
batch.vertex(x, y);

// Vamos desenhar a letra V.
x = 0.922495f;
w = 0.008192f;
y = 0.000000f;
h = 0.678571f;

// Se temos 6 vértices então devemos aplicar 6 vezes coordenadas uv.
// Como é texturizado devemos pasar as coordenadas normalisadas, se não tiver textura inclusa é só passar 0.0f 6 vezes.
batch.coords(x, y);
batch.coords(x, y + h);
batch.coords(x + w, y + h);
batch.coords(x + w, y + h);
batch.coords(x + w, y);
batch.coords(x, y);

batch.next(); // Se você quiser desenhar 30 rects é só pegar esse sub-segmento de (instance() - next()) calls e replicar.

// e.g
// você colocou todo o código acima dentro de uma função ou metódo com parametros para apenas a posição.
// então você pode invocar muitas vezes.
push_rect(20, 50);
push_rect(90, 80);
push_rect(700, 250);

batch.revoke(); // Finalizamos esse segmento.
```
![Alt text](splash/splash-texture.png?raw=true)

Se você quiser ver um exemplo real recomendo olhar a pasta `test/` do projeto, no `main.cpp` você pode ver como usar as features `dynamic_batching` e `font_renderer` de forma otimizada.

Aqui irei explicar como usar o `dynamic_batching` com multiplas instâncias.
`Observação: Multiplas instâncias não tem nada haver com instanced rendering, amogpu ainda não tem essa feature.`

```c++
// ...
// Entretanto se você querer desenhar multiplos shapes na tela em uma unica instância, você tem que especificar o tamanho na hora de enviar as vértices.
// e.g você dá 18 vértices e 18 coordenadas em uma unica instância, mas o tamanho de cada shape é unico, então você altera a posição
// dá vértice e o tamanho de cada mesh, então ai que entra o factor, você tem que colocar no paramêtro alguma coisa que vá fazer sentido
// para o dynamic batch alterar o buffer.
// ...

batch.instance(20, 20);
batch.fill(1.0f, 1.0f, 1.0f, 1.0f, 1.0f); // white;

float x = 0;
float y = 0;

float w = 30;
float h = 30;

for (uint8_t i = 0; i < 5; i++) {
  batch.vertex(x, y);
  batch.vertex(x, y + h);
  batch.vertex(x + w, y + h);
  batch.vertex(x + w, y + h);
  batch.vertex(x + w, y);
  batch.vertex(x, y);
  
  batch.coords(0.0f, 0.0f);
  batch.coords(0.0f, 0.0f);
  batch.coords(0.0f, 0.0f);
  batch.coords(0.0f, 0.0f);
  batch.coords(0.0f, 0.0f);
  batch.coords(0.0f, 0.0f);
  
  x += w + 5;
}

batch.factor(x / 5); // Por que x / 5? Se o tamanho de algum shape mudar, então o buffer precisa mudar.
batch.next();
```
![Alt text](/splash/splash-multi-instances.png?raw=true)

--- 
# Font Renderer

```c++
#include <amogpu/amogpu.hpp>

dynamic_batching batch;
font_renderer f_renderer;

// Se você quiser alterar o tamanho ou mudar de fonte é só rodar esse método.
f_renderer.load("path/to/font.ttf", 18);
f_renderer.from(&batch); // isso vai dizer pro font renderer qual dynamic batch usar.

// Se você quiser deixar automatico, é só fazer.
f_renderer.from(amogpu::invoked);

batch.invoke();
f_renderer.render("hi sou linwda", 10, 10, amogpu::vec4(1.0f, 1.0f, 1.0f, 1.0f));
batch.revoke();

// Ai no loop você dá draw.
while (true) {
    batch.draw();
}

// Você também pode fazer combinações.
// e.g
dynamic_batch batch2;
f_renderer.from(&batch);

// Você pode chamar o batch aplicado pelo método batch.
f_renderer.batch()->invoke();
f_renderer.render("vwc é linda(o)", 10, 10 + 1 + f_renderer.get_text_height(), amogpu::vec4(1.0f, 1.0f, 1.0f, 1.0f));
f_renderer.batch()->revoke();

// Ai no loop você faz.
while (true) {
     f_renderer.batch()->draw();
}
```
![Alt text](splash/splash-font-rendering.png?raw=true)
