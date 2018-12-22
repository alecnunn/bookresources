class Plp {
public:
  explicit Plp(const char *file_name);
  void start(const View &view);
  bool step();
  enum {DEFAULT_BUDGET = 10000};
  enum {NS_CLEAN, NS_PROJECTED, NS_ENQUEUED,
     NS_UNSEEN};
protected:
  void front_push(OctreeNode *);
  OctreeNode *front_pop();
  void project(OctreeNode *node);
  void add_neighbors_to_front(OctreeNode *node);
  void find_visible_front();
  typedef std::set<OctreeNode *, 
    OctreeNode::CompareSolidity> plp_front_t;
  plp_front_t _front;
  std::vector<OctreeNode *> _visible_set;
  unsigned _budget, _num_triangles_rendered;
  unsigned _time_stamp;
  Octree _octree;
  const View *_view;
  OctreeNode *_closest_leaf;
  ImageTileSet _tiles;
};

Plp::Plp(const char *file_name)
  : _budget(DEFAULT_BUDGET),
   _num_triangles_rendered(0), _time_stamp(0),
   _view(NULL), _closest_leaf(NULL)
{
  _octree.read(file_name);
}

void Plp::start(const View &view)
{
  _view = &view;
  _num_triangles_rendered = 0;
  _time_stamp = 0;
  reset_node_states();
  _front.clear();
  _visible_set.clear();
  _closest_leaf = _octree.find_closest_leaf(_view);
  if (_closest_leaf == NULL)
    return;
  _closest_leaf->set_solidity(0.0);
  _closest_leaf->set_layer(0);
  front_push(_closest_leaf);
}

bool Plp::step()
{
  if (_front.empty())
    return false;
  OctreeNode *node = front_pop();
  project(node);
  add_neighbors_to_front(node);
  if (_num_triangles_rendered >= _budget)
    return false;
  return true;
}

void Plp::front_push(OctreeNode *node)
{
  node->set_time_stamp(_time_stamp);
  _time_stamp++;
  _front.insert(node);
  set_node_state(node, NS_ENQUEUED);
}

// returns node most likely to be visible
OctreeNode *Plp::front_pop()
{
  OctreeNode *node = *(_front.begin());
  _front.erase(_front.begin());
  return node;
}

void Plp::project(OctreeNode *node)
{
  set_node_state(node, NS_PROJECTED);
  if (_view->camera().sees_ignoring_near_plane(
      node->bounding_box()) {
    _visible_set.push_back(node);
    _num_triangles_rendered +=
      node->num_triangles();
  }
}

void Plp::add_neighbors_to_front(OctreeNode *node)
{
  const vector<OctreeNode *> &neighbors =
    node->neighbors();
  for (unsigned i = 0; i < neighbors.size(); i++) {
    OctreeNode *neighbor = neighbors[i];
    if (node_state(neighbor) != NS_CLEAN)
      continue;
    if (!is_potentially_visible(neighbor)) {
      set_node_state(neighbor, NS_UNSEEN);
      continue;
    }
    neighbor->set_layer(node->layer() + 1);
    neighbor->set_solidity(neighbor->layer());
    front_push(neighbor);
  }
}
