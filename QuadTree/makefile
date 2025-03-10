CC      = g++
CFLAGS  = -O3 -I/usr/local/include/
LDFLAGS = -L/usr/local/lib/


SOURCES = containers/relation.cpp quadTree/QuadTree.cpp
OBJECTS = $(SOURCES:.cpp=.o)
	
	
all: oneLevel twoLevel twoLevelPlus rtree quadTree refine batch update

oneLevel: $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJECTS) main_onelevel.cpp -o oneLevel $(LDADD)

twoLevel: $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJECTS) main_twolevel.cpp -o twoLevel $(LDADD)

twoLevelPlus: $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJECTS) main_twolevel_plus.cpp -o twoLevelPlus $(LDADD)

rtree: $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJECTS) main_rtree.cpp -o rtree $(LDADD)

quadTreePoints: $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJECTS) main_quadtree_points.cpp -o qt_points $(LDADD)

quadTreeShapes: $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJECTS) main_quadtree_shapes.cpp -o qt_shapes $(LDADD)

refine: $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJECTS) main_refine.cpp -o refine $(LDADD)
	
batch: $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJECTS) main_batch.cpp -o batch $(LDADD)

update: $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJECTS) main_update.cpp -o update $(LDADD)
	
.cpp.o:
	$(CC) $(CFLAGS) -c $< -o $@

.cc.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf containers/*.o
	rm -rf partitioning/*.o
	rm -rf quadTree/*.o
	rm -f twoLevel
	rm -f twoLevelPlus
	rm -f oneLevel
	rm -f rtree
	rm -f qt*
	rm -f refine
	rm -f batch
	rm -f update
