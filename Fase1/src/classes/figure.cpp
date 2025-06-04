#include "figure.hpp"

// Construtor
Figure::Figure()
    : points(newL()), vertices(newL()), normals(newL()), textures(newL()), triangles(newL()) {
}

// Destrutor
Figure::~Figure() {
    deleteFigure();
}

// Adiciona um ponto à lista de pontos
void Figure::addPoint(Point p) {
    addValueList(points, p); // Aloca no heap
}

// Adiciona um vértice
void Figure::addVertex(Point p) {
    addValueList(vertices, p);
}

// Adiciona uma normal
void Figure::addNormal(Point p) {
    addValueList(normals, p);
}

// Adiciona coordenadas de textura
void Figure::addTexture(float u, float v) {
    float* tex = new float[2] { u, v };
    addValueList(textures, tex);
}

// Adiciona um triângulo (índices dos vértices)
void Figure::addTriangle(int v1, int v2, int v3) {
    int* tri = new int[3] { v1, v2, v3 };
    addValueList(triangles, tri);
}

// Retorna a lista de pontos
List Figure::getPoints() const {
    return points;
}

// Retorna a lista de vértices
List Figure::getVertices() const {
    return vertices;
}

// Retorna a lista de normais
List Figure::getNormals() const {
    return normals;
}

// Retorna a lista de texturas
List Figure::getTextures() const {
    return textures;
}

// Retorna a lista de triângulos
List Figure::getTriangles() const {
    return triangles;
}

void Figure::figureToFile(const char* path) {
    if (!this || !this->points || !path) {
        printf("Impossivel criar ficheiro devido a argumentos inválidos\n");
        return;
    }

    FILE* file = fopen(path, "w");
    if (!file) {
        printf("Ocorreu um erro na abertura do ficheiro '%s'\n", path);
        return;
    }

    fprintf(file, "%lu\n", size(this->points));

    List temp = this->points;
    while (temp) {
        Point p = (Point)getData(temp);
        if (p) {
            fprintf(file, "%.3f,%.3f,%.3f\n", getX(p), getY(p), getZ(p));
        }
        temp = getNext(temp);
    }

    fclose(file);
}

// Carrega uma figura de um arquivo
Figure Figure::figureFromFile(const char* path) {
    Figure fig;
	FILE* file = fopen(path, "r");
    
	if (!file) {
		printf("Impossível carregar figura devido a argumentos inválidos\n");
		return fig;
	}

    int vertexs;
	if (fscanf(file, "%d", &vertexs) != 1) {
		printf("Erro ao ler o número de vértices\n");
		fclose(file);
		return fig;
	}

	float x, y, z;
	for (int i = 0; i < vertexs; i++) {
		if (fscanf(file, "%f,%f,%f", &x, &y, &z) == 3) {
			addValueList(fig.points, point(x, y, z));
        }
        else {
			printf("Erro ao ler o vértice %d\n", i);
        }
	}
	fclose(file);
	return fig;
}

Figure::Figure(Figure&& other) noexcept
    : points(other.points), vertices(other.vertices), normals(other.normals), textures(other.textures), triangles(other.triangles) {
    other.points = nullptr;
    other.vertices = nullptr;
    other.normals = nullptr;
    other.textures = nullptr;
    other.triangles = nullptr;
}

Figure& Figure::operator=(Figure&& other) noexcept {
    if (this != &other) {
        deleteFigure();
        points = other.points;
        vertices = other.vertices;
        normals = other.normals;
        textures = other.textures;
        triangles = other.triangles;
        other.points = nullptr;
        other.vertices = nullptr;
        other.normals = nullptr;
        other.textures = nullptr;
        other.triangles = nullptr;
    }
    return *this;
}

// Deleta a memória usada pela figura
void Figure::deleteFigure() {
    printf("Deletando figura\n");
    if (points) {
        printf("Deletando pontos\n");
        freeL(points);
    }
    if (vertices) {
        printf("Deletando vértices\n");
        freeL(vertices);
    }
    if (normals) {
        printf("Deletando normais\n");
        freeL(normals);
    }
    if (textures) {
        printf("Deletando texturas\n");
        freeL(textures);
    }
    if (triangles) {
        printf("Deletando triângulos\n");
        freeL(triangles);
    }
}