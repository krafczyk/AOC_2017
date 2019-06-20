#include <algorithm>
#include <ostream>

//template<typename T>
//concept Iterable 

#if defined(__GNUC__) && not defined(YCM)
// Shweeeeeeeeet

template<typename T>
concept bool ConstIterable = requires(T cont) {
	{ cont.cbegin() } -> typename T::const_iterator;
	{ cont.cend() } -> typename T::const_iterator;
};

template<typename T>
concept bool Iterable = requires(T cont) {
	{ cont.begin() } -> typename T::iterator;
	{ cont.end() } -> typename T::iterator;
};

template<ConstIterable container, typename type>
bool hasElement(const container& cont, const type& el) {
	if(std::find(cont.begin(), cont.end(), el) == cont.end()) {
		return false;
	} else {
		return true;
	}
}

#else
// BOOORING
template<typename iter, typename type>
bool hasElementHelper(const iter& begin, const iter& end, const type& el) {
	if(std::find(begin, end, el) == end) {
		return false;
	} else {
		return true;
	}
}

template<typename container, typename type>
bool hasElement(const container& cont, const type& el) {
	return hasElementHelper(cont.begin(), cont.end(), el);
}
#endif

template<typename C, typename F>
void ConstForEach(const C& container, F functor) {
	std::for_each(container.cbegin(), container.cend(), functor);
}

template<typename C, typename F>
void ForEach(C& container, F functor) {
	std::for_each(container.begin(), container.end(), functor);
}

template<typename C, typename type>
void removeFirst(C& container, const type& el) {
	for(auto it = container.begin(); it != container.end(); ++it) {
		if(*it == el) {
			container.erase(it);
			break;
		}
	}
}

template<typename C, typename type>
void removeAll(C& container, const type& el) {
	for(auto it = container.begin(); it != container.end();) {
		if(*it == el) {
			it = container.erase(it);
		} else {
			++it;
		}
	}
}

template<typename C, typename F>
void removeIf(C& container, F functor) {
    std::remove_if(container.begin(), container.end(), functor);
}

template<typename T>
class array_2d {
    public:
        array_2d(size_t nx=0, size_t ny=0) {
            this->nx = nx;
            this->ny = ny;
            initialize_internal();
        }
        ~array_2d() {
            if(array != nullptr) {
                delete [] array;
            }
        }
        void init(size_t nx, size_t ny) {
            this->nx = nx;
            this->ny = ny;
            initialize_internal();
        }
        T& assign(size_t i, size_t j) {
            return this->array[j*nx+i];
        }
        T operator()(size_t i, size_t j) const {
            return this->array[j*nx+i];
        }
        void print(std::ostream& out) const {
            for(size_t j = 0; j < ny; ++j) {
                for(size_t i = 0; i < nx; ++i) {
                    out << (*this)(i,j);
                }
                out << std::endl;
            }
        }
        size_t width() const {
            return this->nx;
        }
        size_t height() const {
            return this->ny;
        }
    private:
        size_t nx;
        size_t ny;
        T* array;

        void initialize_internal() {
            if((this->nx == 0)||(this->ny == 0)) {
                array = nullptr;
            } else {
                array = new T[nx*ny];
            }
        }
};

template<typename T>
class array_3d {
    public:
        array_3d(size_t nx=0, size_t ny=0, size_t nz = 0) {
            this->nx = nx;
            this->ny = ny;
            this->nz = nz;
            initialize_internal();
        }
        ~array_3d() {
            if(array != nullptr) {
                delete [] array;
            }
        }
        void init(size_t nx, size_t ny, size_t nz) {
            this->nx = nx;
            this->ny = ny;
            this->nz = nz;
            initialize_internal();
        }
        T& assign(size_t i, size_t j, size_t k) {
            return this->array[k*nx*ny+j*nx+i];
        }
        T operator()(size_t i, size_t j, size_t k) const {
            return this->array[k*nx*ny+j*nx+i];
        }
        void print(std::ostream& out) const {
            for(size_t k = 0; k < nz; ++k) {
                for(size_t j = 0; j < ny; ++j) {
                    for(size_t i = 0; i < nx; ++i) {
                        out << (*this)(i,j);
                    }
                    out << std::endl;
                }
                out << std::endl;
            }
        }
    private:
        size_t nx;
        size_t ny;
        size_t nz;
        T* array;

        void initialize_internal() {
            if((this->nx == 0)||(this->ny == 0)||(this->nz == 0)) {
                array = nullptr;
            } else {
                array = new T[nx*ny*nz];
            }
        }
};

template<class T>
class map_val {
    public:
        map_val() {
            this->value = T();
            this->set = false;
        }
        map_val& operator=(const T& new_val) {
            this->value = new_val;
            this->set = true;
            return *this;
        }
        T value;
        bool set;
};

template<class T>
class point2d {
    public:
        point2d(T x=0, T y=0) {
            this->x = x;
            this->y = y;
        }
        point2d(const point2d& rhs) {
            set_equal(rhs);
        }
        point2d& operator=(const point2d& rhs) {
            set_equal(rhs);
            return *this;
        }
        point2d operator+(const point2d& rhs) const {
            return point2d(x+rhs.x,y+rhs.y);
        }
        point2d operator-(const point2d& rhs) const {
            return point2d(x-rhs.x,y-rhs.y);
        }
        template<class U>
        point2d operator*(U a) const {
            return point2d(x*a,y*a);
        }
        T x;
        T y;
    private:
        void set_equal(const point2d& rhs) {
            this->x = rhs.x;
            this->y = rhs.y;
        }
};

template<class T>
class point3d {
    public:
        point3d(T x=0, T y=0, T z=0) {
            this->x = x;
            this->y = y;
            this->z = z;
        }
        point3d(const point3d& rhs) {
            set_equal(rhs);
        }
        point3d& operator=(const point3d& rhs) {
            set_equal(rhs);
            return *this;
        }
        point3d operator+(const point3d& rhs) const {
            return point3d(x+rhs.x,y+rhs.y,z+rhs.z);
        }
        point3d operator-(const point3d& rhs) const {
            return point3d(x-rhs.x,y-rhs.y,z-rhs.z);
        }
        template<class U>
        point3d operator*(U a) const {
            return point3d(x*a,y*a,z*a);
        }
        T x;
        T y;
        T z;
    private:
        void set_equal(const point3d& rhs) {
            this->x = rhs.x;
            this->y = rhs.y;
            this->z = rhs.z;
        }
};

int pair_hash(int x, int y) {
    // Here we use something similar to cantor pairing.
    // https://stackoverflow.com/questions/919612/mapping-two-integers-to-one-in-a-unique-and-deterministic-way
    unsigned int A = (x >= 0 ? 2*x : -2*x-1);
    unsigned int B = (y >= 0 ? 2*y : -2*y-1);
    int C = (int)((A >= B ? A*A+A+B : A+B*B)/2);
    return ((x < 0 && y < 0) || (x >= 0 && y >= 0) ? C : -C - 1);
}

long pair_hash_l(long x, long y) {
    // Here we use something similar to cantor pairing.
    // https://stackoverflow.com/questions/919612/mapping-two-integers-to-one-in-a-unique-and-deterministic-way
    unsigned long A = (x >= 0 ? 2*x : -2*x-1);
    unsigned long B = (y >= 0 ? 2*y : -2*y-1);
    long C = (long)((A >= B ? A*A+A+B : A+B*B)/2);
    return ((x < 0 && y < 0) || (x >= 0 && y >= 0) ? C : -C - 1);
}

template<class T>
T fetch_value(const std::string& str) {
    std::istringstream iss(str);
    T val = 0;
    iss >> val;
    return val;
}
