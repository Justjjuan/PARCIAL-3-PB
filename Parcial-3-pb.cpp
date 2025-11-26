#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
using namespace std;

struct Usuario {
    int id;
    string nombre;
    string correo;
    string contrasena;
    string direccion;
    string metodoPago;
};

struct Producto {
    int id;
    string nombre;
    string descripcion;
    double precio;
    int stock;
};

struct ItemCarrito {
    Producto producto;
    int cantidad;
};

struct Comentario {
    int id;
    string producto;
    string usuario;
    string comentario;
    string fecha;
};

int convertirEntero(const string &texto) {
    int valor = 0;
    stringstream ss(texto);
    ss >> valor;
    return valor;
}

double convertirDouble(const string &texto) {
    double valor = 0;
    stringstream ss(texto);
    ss >> valor;
    return valor;
}

vector<Usuario> cargarUsuarios(const string &nombreArchivo) {
    vector<Usuario> usuarios;
    ifstream archivo(nombreArchivo.c_str());
    string linea;
    getline(archivo, linea);

    while (getline(archivo, linea)) {
        stringstream ss(linea);
        Usuario u;
        string idStr;
        getline(ss, idStr, ',');
        getline(ss, u.nombre, ',');
        getline(ss, u.correo, ',');
        getline(ss, u.contrasena, ',');
        getline(ss, u.direccion, ',');
        getline(ss, u.metodoPago, ',');
        u.id = convertirEntero(idStr);
        usuarios.push_back(u);
    }
    return usuarios;
}

vector<Producto> cargarProductos(const string &nombreArchivo) {
    vector<Producto> productos;
    ifstream archivo(nombreArchivo.c_str());
    string linea;
    getline(archivo, linea);

    while (getline(archivo, linea)) {
        stringstream ss(linea);
        Producto p;
        string idStr, precioStr, stockStr;
        getline(ss, idStr, ',');
        getline(ss, p.nombre, ',');
        getline(ss, p.descripcion, ',');
        getline(ss, precioStr, ',');
        getline(ss, stockStr, ',');
        p.id = convertirEntero(idStr);
        p.precio = convertirDouble(precioStr);
        p.stock = convertirEntero(stockStr);
        productos.push_back(p);
    }
    return productos;
}

vector<Comentario> cargarComentarios(const string &nombreArchivo) {
    vector<Comentario> comentarios;
    ifstream archivo(nombreArchivo.c_str());
    if (!archivo) return comentarios;
    string linea;
    getline(archivo, linea); 

    while (getline(archivo, linea)) {
        if (linea.find_first_not_of(" \t\r\n") == string::npos) continue;
        vector<string> tokens;
        string token;
        stringstream ss(linea);
        while (getline(ss, token, ',')) tokens.push_back(token);

        if (tokens.size() < 5) continue;

        Comentario c;
        c.id = convertirEntero(tokens[0]);
        c.producto = tokens[1];
        c.usuario = tokens[2];
        string texto = tokens[3];
        for (size_t i = 4; i + 1 < tokens.size(); ++i) texto += "," + tokens[i];
        c.comentario = texto;
        c.fecha = tokens.back();
        comentarios.push_back(c);
    }
    archivo.close();
    return comentarios;
}

Usuario *iniciarSesion(vector<Usuario> &usuarios, const string &correo, const string &contrasena) {
    for (size_t i = 0; i < usuarios.size(); ++i)
        if (usuarios[i].correo == correo && usuarios[i].contrasena == contrasena)
            return &usuarios[i];
    return NULL;
}

vector<Producto> listarProductosBajoStock(const vector<Producto> &productos) {
    vector<Producto> bajos;
    for (size_t i = 0; i < productos.size(); ++i)
        if (productos[i].stock < 15)
            bajos.push_back(productos[i]);
    return bajos;
}

void guardarCarritoArchivo(const Usuario &usuario, const vector<ItemCarrito> &carrito) {
    ofstream archivo("Carrito.txt", ios::app);
    for (size_t i = 0; i < carrito.size(); ++i) {
        double subtotal = carrito[i].cantidad * carrito[i].producto.precio;
        archivo << usuario.id << ","
                << usuario.nombre << ","
                << carrito[i].producto.id << ","
                << carrito[i].producto.nombre << ","
                << carrito[i].cantidad << ","
                << carrito[i].producto.precio << ","
                << subtotal << "\n";
    }
    archivo.close();
}

void mostrarCarritoArchivo() {
    ifstream archivo("Carrito.txt");
    if (!archivo) {
        cout << "No hay carrito registrado.\n";
        return;
    }

    string linea;
    cout << "\n=== CONTENIDO DEL CARRITO GUARDADO ===\n";
    cout << "Usuario | Producto | Cantidad | Precio | Subtotal\n";
    while (getline(archivo, linea)) {
        stringstream ss(linea);
        string idUsuario, nombreUsuario, idProd, nombreProd, cantidad, precio, subtotal;
        getline(ss, idUsuario, ',');
        getline(ss, nombreUsuario, ',');
        getline(ss, idProd, ',');
        getline(ss, nombreProd, ',');
        getline(ss, cantidad, ',');
        getline(ss, precio, ',');
        getline(ss, subtotal, ',');
        cout << nombreUsuario << " | " << nombreProd << " | " << cantidad
             << " | $" << precio << " | $" << subtotal << "\n";
    }
    archivo.close();
}

void mostrarCarrito(const vector<ItemCarrito> &carrito) {
    cout << "\n=== CARRITO DE COMPRAS ===\n";
    double total = 0;
    for (size_t i = 0; i < carrito.size(); ++i) {
        double subtotal = carrito[i].cantidad * carrito[i].producto.precio;
        cout << carrito[i].producto.nombre
             << " (Precio: $" << carrito[i].producto.precio
             << ", Cantidad: " << carrito[i].cantidad
             << ") = $" << subtotal << "\n";
        total += subtotal;
    }
    cout << "TOTAL: $" << total << "\n";
}


void reporteProductosMenorStock(const vector<Producto> &productos) {
    vector<Producto> copia = productos;

    for (size_t i = 0; i < copia.size(); ++i) {
        for (size_t j = i + 1; j < copia.size(); ++j) {
            if (copia[j].stock < copia[i].stock) {
                Producto temp = copia[i];
                copia[i] = copia[j];
                copia[j] = temp;
            }
        }
    }
    cout << "\n=== 5 PRODUCTOS CON MENOR STOCK ===\n";
    cout << "idProducto | nombre | stock\n";
    for (size_t i = 0; i < 5 && i < copia.size(); ++i) {
        cout << copia[i].id << " | " << copia[i].nombre << " | " << copia[i].stock << "\n";
    }
}


void reporteComentariosPorFecha(const vector<Comentario> &comentarios) {
    string fechaBuscada;
    cout << "Ingrese la fecha (YYYY-MM-DD): ";
    cin >> fechaBuscada;
    int contador = 0;
    for (size_t i = 0; i < comentarios.size(); ++i)
        if (comentarios[i].fecha == fechaBuscada)
            contador++;
    cout << "\n=== COMENTARIOS POR FECHA ===\n";
    cout << "Fecha: " << fechaBuscada << " | Cantidad: " << contador << "\n";
}


void reportePrecioMaxMin(const vector<Producto> &productos) {
    if (productos.empty()) {
        cout << "No hay productos.\n";
        return;
    }
    double maxP = productos[0].precio;
    double minP = productos[0].precio;
    for (size_t i = 1; i < productos.size(); ++i) {
        if (productos[i].precio > maxP) maxP = productos[i].precio;
        if (productos[i].precio < minP) minP = productos[i].precio;
    }
    cout << "\n=== PRECIO MÁXIMO Y MÍNIMO ===\n";
    cout << "Precio máximo: $" << maxP << "\n";
    cout << "Precio mínimo: $" << minP << "\n";
}

int main() {
    vector<Usuario> usuarios = cargarUsuarios("Usuarios.txt");
    vector<Producto> productos = cargarProductos("Productos.txt");
    vector<Comentario> comentarios = cargarComentarios("Comentarios.txt");
    vector<ItemCarrito> carrito;

    string correo, contrasena;
    cout << "Correo electrónico: ";
    getline(cin, correo);
    cout << "Contraseña: ";
    getline(cin, contrasena);

    Usuario *usuarioActual = iniciarSesion(usuarios, correo, contrasena);
    if (!usuarioActual) {
        cout << "Usuario o contraseña inválidos.\n";
        return 0;
    }

    int opcion;
    do {
        cout << "\n=== MENÚ PRINCIPAL ===\n";
        cout << "1. Listar productos con stock menor a 15\n";
        cout << "2. Listar todos los usuarios\n";
        cout << "3. Agregar producto al carrito\n";
        cout << "4. Ver carrito actual\n";
        cout << "5. Guardar carrito en archivo\n";
        cout << "6. Ver carrito guardado (archivo)\n";
        cout << "7. Reportes\n";
        cout << "0. Salir\n";
        cout << "Seleccione una opción: ";
        cin >> opcion;
        cin.ignore();

        switch (opcion) {
        case 1: {
            vector<Producto> bajos = listarProductosBajoStock(productos);
            cout << "\n=== PRODUCTOS CON STOCK < 15 ===\n";
            for (size_t i = 0; i < bajos.size(); ++i)
                cout << bajos[i].id << " | " << bajos[i].nombre
                     << " | " << bajos[i].descripcion
                     << " | Precio: $" << bajos[i].precio
                     << " | Stock: " << bajos[i].stock << "\n";
            break;
        }
        case 2: {
            cout << "\n=== LISTA DE USUARIOS ===\n";
            for (size_t i = 0; i < usuarios.size(); ++i)
                cout << usuarios[i].id << " | " << usuarios[i].nombre
                     << " | " << usuarios[i].correo
                     << " | Dirección: " << usuarios[i].direccion
                     << " | Pago: " << usuarios[i].metodoPago << "\n";
            break;
        }
        case 3: {
            int idProd, cant;
            cout << "Ingrese el ID del producto: ";
            cin >> idProd;
            cout << "Cantidad: ";
            cin >> cant;
            cin.ignore();

            bool encontrado = false;
            for (size_t i = 0; i < productos.size(); ++i) {
                if (productos[i].id == idProd) {
                    ItemCarrito item;
                    item.producto = productos[i];
                    item.cantidad = cant;
                    carrito.push_back(item);
                    cout << "Producto agregado al carrito.\n";
                    encontrado = true;
                    break;
                }
            }
            if (!encontrado)
                cout << "Producto no encontrado.\n";
            break;
        }
        case 4:
            mostrarCarrito(carrito);
            break;
        case 5:
            guardarCarritoArchivo(*usuarioActual, carrito);
            cout << "Carrito guardado correctamente en Carrito.txt.\n";
            break;
        case 6:
            mostrarCarritoArchivo();
            break;
        case 7: {
            int op;
            do {
                cout << "\n=== MENÚ DE REPORTES ===\n";
                cout << "1. 5 productos con menor stock\n";
                cout << "2. Cantidad de comentarios por fecha\n";
                cout << "3. Precio máximo y mínimo\n";
                cout << "0. Volver\n";
                cout << "Seleccione una opción: ";
                cin >> op;
                cin.ignore();

                switch (op) {
                case 1:
                    reporteProductosMenorStock(productos);
                    break;
                case 2:
                    reporteComentariosPorFecha(comentarios);
                    break;
                case 3:
                    reportePrecioMaxMin(productos);
                    break;
                case 0:
                    break;
                default:
                    cout << "Opción no válida.\n";
                }
            } while (op != 0);
            break;
        }
        case 0:
            cout << "Saliendo del sistema...\n";
            break;
        default:
            cout << "Opción no válida.\n";
        }
    } while (opcion != 0);

    return 0;
}
