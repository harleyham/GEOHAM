"# GEOHAM" 
SW de geração e tratamento de mosaicos
Versão 0.668

V 0.668 (MAI 2023)
a)	Criação do arquivo de configuração, que é atualizado quando modificações são feitas nas configurações;
b)	BUG Corrigido: Quando trocava de diretório, ao ler uma imagem o SW dava um crash (problema com DestroyWindow do OpenCV);
c)	O KML escrito coloca como altura do voo a média, e não a altura real de cada foto;
d)	BUG encontrado -> Não está lendo as imagens após a conversão de PPK usando o ENLID. Pelo analisado, há uma alteração nas TAGS EXIF que a biblioteca GDAL não consegue ler; (possível correção seria a alteração de ler com GDAL e passar a ler com EXIFTOOLS)
e)	Correção de vários BUGS na janela do Manual. Ainda não corrigido o BUG de quando fecha a janela, ao reabrir não aparece o arquivo e sim janela vazia;


V 0.667 (MAR 2023)
a)	BUG encontrado: Ao fechar a janela do manual em PDF, quando abre de novo não aparece o arquivo, apenas a janela vazia;
b)	BUG corrigido -> quando seleciona outro diretório não abre a última imagem lida;
c)	BUG corrigido -> quando se limpava o diretório de processamento não apagava a caixa de texto do número de imagens selecionadas;
d)	Implementado calcular o ângulo de voo na Ida e na Volta;
e)	Implementado identificar quais imagens foram tiradas com o drone voando nas direções de IDA, VOLTA ou fazendo curvas;
f)	Implementado mostrar estas novas informações quando um diretório é selecionado;
g)	Implementado mostrar sobre a imagem mesclada o ângulo de voo e a média do ângulo;
h)	Implementado mesclar:
a.	Utilizando todas as imagens processadas;
b.	Selecionar se serão utilizadas as imagens tiradas com o drone fazendo curva;
c.	Utilizando as imagens com o drone voando em uma das direções (neste caso é possível selecionar se serão utilizadas as imagens da primeira e última linha em conjunto)
i)	Implementado fazer a mesclagem com transparência;
j)	Implementado ler imagens JPG da câmera Parrot Sequoia a fim de poder verificar o nome da imagem em sua localização. Implementação feita para identificar quais imagens devem ser retiradas do processamento devido a serem tiradas durante o deslocamento até o local do imageamento;
k)	Implementado fazer pilha de imagens PROC para poder escolher que imagem fica por cima;
l)	Adaptações para QT6;
m)	Troca do módulo de mostrar PDF;
n)	Novo Mapa Mental https://gitmind.com
o)	Implementado obter Metadados de uma imagem processada através da list RESULT
p)	Implementado colocar no EXIF da imagem mesclada informações:
a.	EXIF_SOFTWARE
b.	Altitude
c.	Ângulo_IDA
d.	Ângulo_Volta
e.	EXIF_Make
f.	EXIF_Model
g.	Num_Images
h.	Sentido
