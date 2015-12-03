GMATH_FILES = obb.cpp
GMATH_OBJ = $(GMATH_FILES:%.cpp=%.o)

all: libgomezmath.a

libgomezmath.a: $(GMATH_OBJ)
	rm -f $@
	ar cq $@ $(GMATH_OBJ)

clean:
	rm -f libgomezmath.a $(GMATH_OBJ)
