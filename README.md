# 3D Maze Generator
A UI interface for constructing **3D mazes** and exporting them as models. Built with Qt Widgets 6.9.2

## UI overview
<img width="1127" height="704" alt="image" src="https://github.com/user-attachments/assets/ad865524-ee25-489a-ad55-2be324dcca56" />
The UI window features:
- **3D visualization** of the model on the left
- **Configuration panel** on the right, which contains the following:
  - **Rooms per axis** - the dimensions of the maze structure itself
  - **Relative wall width** - coefficient which tells the program how big the walls should be, compared to room sizes
  - **Openings list**  - a list which keeps track of all openings on the model, can be manipulated with the appropriate **Add** and **Remove** buttons. You may select quads on the model to append or remove them to the list
  - **Generate** button - generates new pathways inside the maze (without loops)
  - **Mesh size** - specifies the output mesh dimensions. When the **linked** checkbox is set to on, the program should keep the original proportions when changing the size
  - **Export** button - exports the model to a Wavefront .OBJ file
