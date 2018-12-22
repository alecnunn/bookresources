void Plp::find_visible_front()
{
  // save masks
  GLboolean cmask[4], dmask;
  glGetBooleanv(GL_COLOR_WRITEMASK, cmask);
  glGetBooleanv(GL_DEPTH_WRITEMASK, &dmask);
  // empty front, and remember nodes to render
  vector<OctreeNode *> nodes;
  while (!_front.empty()) {
    OctreeNode *node = front_pop();
    set_node_state(node, NS_CLEAN);
    nodes.push_back(node);
  }
  // render front
  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
  glDepthMask(GL_FALSE);
  glClear(GL_COLOR_BUFFER_BIT);
  for (unsigned i = 0; i < nodes.size(); i++) {
    OctreeNode *node = nodes[i];
     // 0 is background
    unsigned j = i + 1;
    GLubyte r = j & 0x000000FF;
    GLubyte g = j & 0x0000FF00;
    GLubyte b = j & 0x00FF0000;
    glColor3ub(r, g, b);
      node->render_solid_box();
  }
  // determine visible front
  _tiles.read_active();
  list<ImageTile *>::iterator li =
    _tiles.active_tiles().begin();
  while (li != _tiles.active_tiles().end()) {
    ImageTile *tile = *li;
    Image &img = tile->image();
    GLubyte *p = img.pixels();
    unsigned n = img.num_pixels();
    bool tile_active = false;
    for (unsigned i = 0; i < n; i++) {
      GLubyte r = *p++;
      GLubyte b = *p++;
      GLubyte g = *p++;
      GLubyte a = *p++;
      unsigned j = r | g << 8 | b << 16;
      if (j == 0)
        continue;
      tile_active = true;
      j--;
      OctreeNode *node = nodes[j];
      if (node_state(node) != NS_ENQUEUED)
        front_push(node);
    }
    list<ImageTile *>::iterator t = li;
    li++;
    if (!tile_active)
      _tiles.deactivate(t);
  }
  // restore masks
  glColorMask(cmask[0], cmask[1], cmask[2],
    cmask[3]);
  glDepthMask(dmask);
}

void Plp::conservative_finish()
{
  const Viewport &v = _view->viewport();
  _tiles.realloc(v.x_min(), v.y_min(), v.width(),
    v.height());
  _tiles.activate_all();
  // save state
  glPushAttrib(GL_ENABLE_BIT);
  GLboolean cmask[4], dmask;
  glGetBooleanv(GL_COLOR_WRITEMASK, cmask);
  glGetBooleanv(GL_DEPTH_WRITEMASK, &dmask);
  // compute z-buffer for approximate visible set
  glDisable(GL_LIGHTING);
  glClear(GL_DEPTH_BUFFER_BIT);
  glColorMask(GL_FALSE, GL_FALSE, GL_FALSE,
    GL_FALSE);
  glDepthMask(GL_TRUE);
  for (unsigned i = 0; i < _visible_set.size(); i++)
    _visible_set[i]->render_geometry();
  while (!_front.empty()) {
    find_visible_front();
    // empty front, and remember nodes to project
    vector<OctreeNode *> nodes;
    while (!_front.empty()) {
      OctreeNode *node = front_pop();
      nodes.push_back(node);
    }
    // project nodes, and determine new front
    for (unsigned i = 0; i < nodes.size(); i++)
    {
      OctreeNode *node = nodes[i];
      project(node);
      add_neighbors_to_front(node);
      node->render_geometry();
    }
  }
  // restore state
  glPopAttrib();
  glColorMask(cmask[0], cmask[1], cmask[2],
    cmask[3]);
  glDepthMask(dmask);
}
