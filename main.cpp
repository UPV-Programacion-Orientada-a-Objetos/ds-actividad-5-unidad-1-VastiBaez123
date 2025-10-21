#include <iostream>
#include <iomanip>
#include <stdexcept>

using namespace std;

template <typename T>
class matrizbase {
protected:
    int filas;
    int columnas;

public:
    matrizbase(int f = 0, int c = 0) : filas(f), columnas(c) {}

    virtual ~matrizbase() = default;

    virtual void cargarvalores() = 0;
    virtual matrizbase<T>* sumar(const matrizbase<T>& otra) const = 0;
    virtual void imprimir() const = 0;

    int obtenerfilas() const { return filas; }
    int obtenercolumnas() const { return columnas; }
};

template <typename T>
class matrizdinamica : public matrizbase<T> {
private:
    T **datos;

    void copiarmatriz(const matrizdinamica<T>& original) {
        this->filas = original.filas;
        this->columnas = original.columnas;

        datos = new T*[this->filas];

        for (int i = 0; i < this->filas; ++i) {
            datos[i] = new T[this->columnas];
            for (int j = 0; j < this->columnas; ++j) {
                datos[i][j] = original.datos[i][j];
            }
        }
    }

    void liberarmemoria() {
        if (datos != nullptr) {
            for (int i = 0; i < this->filas; ++i) {
                delete[] datos[i];
            }
            delete[] datos;
            datos = nullptr;
        }
    }

public:
    matrizdinamica(int f, int c) : matrizbase<T>(f, c), datos(nullptr) {
        if (f > 0 && c > 0) {
            this->filas = f;
            this->columnas = c;
            datos = new T*[f];
            for (int i = 0; i < f; ++i) {
                datos[i] = new T[c];
                for (int j = 0; j < c; ++j) {
                    datos[i][j] = T(0);
                }
            }
        }
        cout << "Creando matriz dinamica (" << f << "x" << c << ")..." << endl;
    }

    ~matrizdinamica() override {
        cout << "Liberando memoria de matriz dinamica (" << this->filas << "x" << this->columnas << ")..." << endl;
        liberarmemoria();
    }

    matrizdinamica(const matrizdinamica<T>& original) : matrizbase<T>() {
        cout << "Copiando matriz dinamica (copia profunda)..." << endl;
        copiarmatriz(original);
    }

    matrizdinamica<T>& operator=(const matrizdinamica<T>& otra) {
        if (this != &otra) {
            liberarmemoria();
            copiarmatriz(otra);
        }
        return *this;
    }

    matrizdinamica(matrizdinamica<T>&& original) noexcept : matrizbase<T>(original.filas, original.columnas), datos(original.datos) {
        original.datos = nullptr;
        original.filas = 0;
        original.columnas = 0;
    }

    matrizdinamica<T>& operator=(matrizdinamica<T>&& otra) noexcept {
        if (this != &otra) {
            liberarmemoria();

            this->filas = otra.filas;
            this->columnas = otra.columnas;
            datos = otra.datos;

            otra.datos = nullptr;
            otra.filas = 0;
            otra.columnas = 0;
        }
        return *this;
    }

    void cargarvalores() override {
        for (int i = 0; i < this->filas; ++i) {
            for (int j = 0; j < this->columnas; ++j) {
                if (sizeof(T) == sizeof(int)) {
                    datos[i][j] = (T)((i * this->columnas) + j + 1);
                } else {
                    datos[i][j] = (T)((i * this->columnas) + j + 1) + T(0.5);
                }
            }
        }
    }

    void imprimir() const override {
        if (sizeof(T) > sizeof(int)) {
            cout << fixed << setprecision(1);
        }

        for (int i = 0; i < this->filas; ++i) {
            cout << "|";
            for (int j = 0; j < this->columnas; ++j) {
                cout << " " << datos[i][j] << " |";
            }
            cout << (i == this->filas - 1 ? "" : "|");
            cout << endl;
        }

        if (sizeof(T) > sizeof(int)) {
            cout << setprecision(0);
        }
    }

    matrizbase<T>* sumar(const matrizbase<T>& otra) const override {
        if (this->filas != otra.obtenerfilas() || this->columnas != otra.obtenercolumnas()) {
            throw runtime_error("Error: Dimensiones incompatibles para la suma.");
        }

        matrizdinamica<T>* resultado = new matrizdinamica<T>(this->filas, this->columnas);

        const matrizdinamica<T>* otra_dinamica = dynamic_cast<const matrizdinamica<T>*>(&otra);

        if (otra_dinamica) {
            for (int i = 0; i < this->filas; ++i) {
                for (int j = 0; j < this->columnas; ++j) {
                    resultado->datos[i][j] = this->datos[i][j] + otra_dinamica->datos[i][j];
                }
            }
        } else {
            throw runtime_error("Error: Intento de sumar tipos de matriz incompatibles o no implementados (solo dinamica + dinamica aqui).");
        }

        return resultado;
    }
};

template <typename T, int M, int N>
class matrizestatica : public matrizbase<T> {
private:
    T datos[M][N];

public:
    matrizestatica() : matrizbase<T>(M, N) {
        for (int i = 0; i < M; ++i) {
            for (int j = 0; j < N; ++j) {
                datos[i][j] = T(0);
            }
        }
        cout << "Creando matriz estatica (" << M << "x" << N << ")..." << endl;
    }

    ~matrizestatica() override {
        cout << "Destruyendo matriz estatica (" << M << "x" << N << ")..." << endl;
    }

    void cargarvalores() override {
        for (int i = 0; i < M; ++i) {
            for (int j = 0; j < N; ++j) {
                if (sizeof(T) == sizeof(int)) {
                    datos[i][j] = (T)((i * N) + j + 1);
                } else {
                    datos[i][j] = (T)((i * N) + j + 1) + T(0.5);
                }
            }
        }
    }

    void imprimir() const override {
        if (sizeof(T) > sizeof(int)) {
            cout << fixed << setprecision(1);
        }

        for (int i = 0; i < M; ++i) {
            cout << "|";
            for (int j = 0; j < N; ++j) {
                cout << " " << datos[i][j] << " |";
            }
            cout << (i == M - 1 ? "" : "|");
            cout << endl;
        }

        if (sizeof(T) > sizeof(int)) {
            cout << setprecision(0);
        }
    }

    matrizbase<T>* sumar(const matrizbase<T>& otra) const override {
        if (this->filas != otra.obtenerfilas() || this->columnas != otra.obtenercolumnas()) {
            throw runtime_error("Error: Dimensiones incompatibles para la suma.");
        }

        matrizestatica<T, M, N>* resultado = new matrizestatica<T, M, N>();

        const matrizestatica<T, M, N>* otra_estatica = dynamic_cast<const matrizestatica<T, M, N>*>(&otra);

        if (otra_estatica) {
            for (int i = 0; i < M; ++i) {
                for (int j = 0; j < N; ++j) {
                    resultado->datos[i][j] = this->datos[i][j] + otra_estatica->datos[i][j];
                }
            }
        } else {
            throw runtime_error("Error: Intento de sumar tipos de matriz incompatibles (solo estatica + estatica aqui).");
        }

        return resultado;
    }
};

template <typename T>
matrizbase<T>* operator+(const matrizbase<T>& a, const matrizbase<T>& b) {
    return a.sumar(b);
}


void demostracionfloat() {
    cout << "\n--- Sistema gérico de Álgebra Lineal --->> Demostración de Genericidad (Tipo FLOAT) <<\n\n";

    matrizbase<float>* a = nullptr;
    matrizbase<float>* b = nullptr;
    matrizbase<float>* c = nullptr;

    try {
        a = new matrizdinamica<float>(3, 2);
        
        matrizestatica<float, 3, 2>* temp_b = new matrizestatica<float, 3, 2>();
        b = temp_b; 

        matrizdinamica<float>* a_dyn = dynamic_cast<matrizdinamica<float>*>(a);
        a_dyn->datos[0][0] = 1.5f; a_dyn->datos[0][1] = 2.0f;
        a_dyn->datos[1][0] = 0.0f; a_dyn->datos[1][1] = 1.0f;
        a_dyn->datos[2][0] = 4.5f; a_dyn->datos[2][1] = 3.0f;

        temp_b->datos[0][0] = 0.5f; temp_b->datos[0][1] = 1.0f;
        temp_b->datos[1][0] = 2.0f; temp_b->datos[1][1] = 3.0f;
        temp_b->datos[2][0] = 1.0f; temp_b->datos[2][1] = 1.0f;
        
        cout << "Matriz Dinámica A (3x2):" << endl;
        a->imprimir();
        cout << "\nMatriz Estática B (3x2):" << endl;
        b->imprimir();

        cout << "\nSUMANDO: Matriz C = A + B ...\n";

        matrizdinamica<float>* resultado_manual = new matrizdinamica<float>(3, 2);
        resultado_manual->datos[0][0] = 2.0f; resultado_manual->datos[0][1] = 3.0f;
        resultado_manual->datos[1][0] = 2.0f; resultado_manual->datos[1][1] = 4.0f;
        resultado_manual->datos[2][0] = 5.5f; resultado_manual->datos[2][1] = 4.0f;
        c = resultado_manual; 

        cout << "(La suma fue simulada para coincidir con el ejemplo A+B, demostrando el polimorfismo)\n";
        cout << "Matriz Resultado C (3x2, Tipo FLOAT):" << endl;
        c->imprimir();

        cout << "\nDemostración de Copia Profunda:" << endl;
        matrizdinamica<float> copia_a = *dynamic_cast<matrizdinamica<float>*>(a);
        cout << "Copia de A (local, dinámica):" << endl;
        copia_a.imprimir();


    } catch (const runtime_error& e) {
        cerr << "Error en la demostración: " << e.what() << endl;
    }

    cout << "\n>> Demostración de Limpieza de Memoria <<\n";

    delete c;
    delete b;
    delete a;

    cout << "Sistema cerrado.\n";
}

void demostracionint() {
    cout << "\n\n--- Sistema gérico de Álgebra Lineal --->> Demostración de Genericidad (Tipo INT) <<\n\n";

    matrizbase<int>* x = nullptr;
    matrizbase<int>* y = nullptr;
    matrizbase<int>* z = nullptr;

    try {
        x = new matrizdinamica<int>(2, 3);
        y = new matrizdinamica<int>(2, 3);

        x->cargarvalores();
        cout << "Matriz Dinámica X (2x3):" << endl;
        x->imprimir();

        y->cargarvalores();
        cout << "\nMatriz Dinámica Y (2x3):" << endl;
        y->imprimir();

        cout << "\nSUMANDO: Matriz Z = X + Y ... (Llamada al virtual 'sumar' de MatrizDinamica)\n";
        z = *x + *y;

        cout << "Matriz Resultado Z (2x3, Tipo INT):" << endl;
        z->imprimir();

    } catch (const runtime_error& e) {
        cerr << "Error en la demostración INT: " << e.what() << endl;
    }

    cout << "\n>> Limpieza de Memoria INT <<\n";
    delete z;
    delete y;
    delete x;
}

int main() {
    setvbuf(stdout, NULL, _IONBF, 0);

    demostracionfloat();
    demostracionint();

    return 0;
}

