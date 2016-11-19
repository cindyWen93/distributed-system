/* This is the main cpp file of graph_server */
/* Author: YANBIJIA JIN & SONGZI WEN */
/* Date: Sep. 2016 */

/* User can set the port which server is listening, the default port is 8000 */

#include "mongoose.h"
#include "graph.h"

static struct mg_serve_http_opts s_http_server_opts;

Graph graph;

/* this function is used to get node_id from JSON field*/
const char* get_node(const char *p, int & node_id){
  string node;
  while(p!= NULL && (*p > '9' || *p <'0')){p++;}
  while(p!= NULL && (*p <= '9' && *p >= '0')){node.append(1,*(p++));}
  node_id = strtoll(node.c_str(), NULL, 10);
  return p;
}

static void handle_add_node_call(struct mg_connection *nc, struct http_message *hm) {

  const char *p = hm->body.p;
  int node_id; p = get_node(p,node_id);
  int ret = graph.add_node(node_id);

  /* if the node already exists */
  if(ret == 204){
      mg_printf(nc, "%s", "HTTP/1.1 204 No Content\r\nTransfer-Encoding: chunked\r\n\r\n");
  }

  /* on success */
  else{
    mg_printf(nc, "%s", "HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n\r\n");
    mg_printf_http_chunk(nc, "{ \"node_id\": %d }", node_id);
  }
  mg_send_http_chunk(nc, "", 0);
}

static void handle_add_edge_call(struct mg_connection *nc, struct http_message *hm) {
  const char *p = hm->body.p;
  int node_a_id, node_b_id; p = get_node(p,node_a_id); p = get_node(p,node_b_id);
  int ret = graph.add_edge(node_a_id,node_b_id);

  /* if either node doesn't exist, or if node_a_id is the same as node_b_id */
  if(ret == 400){
    mg_printf(nc, "%s", "HTTP/1.1 400 Bad Request\r\nTransfer-Encoding: chunked\r\n\r\n");
  }

  /* if the edge already exists */
  else if(ret == 204){
    mg_printf(nc, "%s", "HTTP/1.1 204 No Content\r\nTransfer-Encoding: chunked\r\n\r\n");
  }

  /* on success */
  else{
    mg_printf(nc, "%s", "HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n\r\n");
    mg_printf_http_chunk(nc, "{ \"node_a_id\": %d, ", node_a_id);
    mg_printf_http_chunk(nc, "\"node_b_id\": %d }", node_b_id);
  }
  mg_send_http_chunk(nc, "", 0);
}

static void handle_remove_node_call(struct mg_connection *nc, struct http_message *hm) {
  const char *p = hm->body.p;
  int node_id; p = get_node(p,node_id);
  int ret = graph.remove_node(node_id);

  /* if the node does not exist */
  if(ret == 400){
      mg_printf(nc, "%s", "HTTP/1.1 400 Bad Request\r\nTransfer-Encoding: chunked\r\n\r\n");
  }

  /* on success */
  else{
    mg_printf(nc, "%s", "HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n\r\n");
  }
  mg_send_http_chunk(nc, "", 0);
}

static void handle_remove_edge_call(struct mg_connection *nc, struct http_message *hm) {
  const char *p = hm->body.p;
  int node_a_id, node_b_id; p = get_node(p,node_a_id); p = get_node(p,node_b_id);
  int ret = graph.remove_edge(node_a_id,node_b_id);

  /* if the edge does not exist */
  if(ret == 400){
      mg_printf(nc, "%s", "HTTP/1.1 400 Bad Request\r\nTransfer-Encoding: chunked\r\n\r\n");
  }

  /* on success */
  else{
    mg_printf(nc, "%s", "HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n\r\n");
  }
  mg_send_http_chunk(nc, "", 0);
}

static void handle_get_node_call(struct mg_connection *nc, struct http_message *hm) {
  const char *p = hm->body.p;
  int node_id; p = get_node(p,node_id);
  bool exists;
  graph.get_node(node_id,exists);

  /* on success*/
  mg_printf(nc, "%s", "HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n\r\n");
  mg_printf_http_chunk(nc, "{ \"in_graph\": %d }", exists);
  mg_send_http_chunk(nc, "", 0); /* Send empty chunk, the end of response */
}

static void handle_get_edge_call(struct mg_connection *nc, struct http_message *hm) {
  const char *p = hm->body.p;
  bool exists;
  int node_a_id, node_b_id; p = get_node(p,node_a_id); p = get_node(p,node_b_id);
  int ret = graph.get_edge(node_a_id,node_b_id,exists);

  /* at least one of the vertices does not exist */
  if(ret == 400){
    mg_printf(nc, "%s", "HTTP/1.1 400 Bad Request\r\nTransfer-Encoding: chunked\r\n\r\n");
  }

  /* on success*/
  else{
    mg_printf(nc, "%s", "HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n\r\n");
    mg_printf_http_chunk(nc, "{ \"in_graph\": %d }", exists);
  }
  mg_send_http_chunk(nc, "", 0);
}

static void handle_get_neighbors_call(struct mg_connection *nc, struct http_message *hm) {
  const char *p = hm->body.p;
  int node_id; p = get_node(p,node_id);
  vector<int> neighbors;
  int ret = graph.get_neighbors(node_id,neighbors);

  /* if the node does not exist*/
  if(ret == 400){
    mg_printf(nc, "%s", "HTTP/1.1 400 Bad Request\r\nTransfer-Encoding: chunked\r\n\r\n");
  }

  /* on success*/
  else{
    mg_printf(nc, "%s", "HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n\r\n");
    mg_printf_http_chunk(nc, "{ \"node_id\": %d,  \"neighbors\": [", node_id);
    for(int i = 0; i < neighbors.size(); i++){
      mg_printf_http_chunk(nc, "%d, ", neighbors[i]);
    }
    mg_printf_http_chunk(nc, "\t\t ] }");
  }
  mg_send_http_chunk(nc, "", 0); /* Send empty chunk, the end of response */
}

static void handle_shortest_path_call(struct mg_connection *nc, struct http_message *hm) {
  const char *p = hm->body.p;
  int node_a_id,node_b_id; p = get_node(p,node_a_id); p = get_node(p,node_b_id);
  int distance;
  int ret = graph.shortest_path(node_a_id,node_b_id,distance);

  /* if either node does not exist */
  if(ret == 400){
    mg_printf(nc, "%s", "HTTP/1.1 400 Bad Request\r\nTransfer-Encoding: chunked\r\n\r\n");
  }
  else if(ret == 204){
    mg_printf(nc, "%s", "HTTP/1.1 204 No Content\r\nTransfer-Encoding: chunked\r\n\r\n");
  }
  /* on success*/
  else{
      mg_printf(nc, "%s", "HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n\r\n");
      mg_printf_http_chunk(nc, "{ \"distance\": %d }", distance);
  }
  mg_send_http_chunk(nc, "", 0);
}

static void ev_handler(struct mg_connection *nc, int ev, void *ev_data) {
  struct http_message *hm = (struct http_message *) ev_data;

  switch (ev) {
    case MG_EV_HTTP_REQUEST:
      if (mg_vcmp(&hm->uri, "/api/v1/add_node") == 0) {
        handle_add_node_call(nc, hm);
      }
      else if (mg_vcmp(&hm->uri, "/api/v1/add_edge") == 0){
        handle_add_edge_call(nc, hm);
      }
      else if (mg_vcmp(&hm->uri, "/api/v1/remove_node") == 0){
        handle_remove_node_call(nc, hm);
      }
      else if (mg_vcmp(&hm->uri, "/api/v1/remove_edge") == 0){
        handle_remove_edge_call(nc, hm);
      }
      else if (mg_vcmp(&hm->uri, "/api/v1/get_node") == 0){
        handle_get_node_call(nc, hm);
      }
      else if (mg_vcmp(&hm->uri, "/api/v1/get_edge") == 0){
        handle_get_edge_call(nc, hm);
      }
      else if (mg_vcmp(&hm->uri, "/api/v1/get_neighbors") == 0){
        handle_get_neighbors_call(nc, hm);
      }
      else if (mg_vcmp(&hm->uri, "/api/v1/shortest_path") == 0){
        handle_shortest_path_call(nc, hm);
      }
      else{
        mg_send_http_chunk(nc, "", 0);
      }
      break;
    default:
          break;
  }
}

int main(int argc, char *argv[]) {
  struct mg_mgr mgr;
  struct mg_connection *nc;
  struct mg_bind_opts bind_opts;
  const char *err_str;
#ifdef MG_ENABLE_SSL
  const char *ssl_cert = NULL;
#endif

  const char *s_http_port = argc > 1 ? argv[1] : "8000";
  mg_mgr_init(&mgr, NULL);

  /* Set HTTP server options */
  memset(&bind_opts, 0, sizeof(bind_opts));
  bind_opts.error_string = &err_str;
#ifdef MG_ENABLE_SSL
  if (ssl_cert != NULL) {
    bind_opts.ssl_cert = ssl_cert;
  }
#endif

  nc = mg_bind_opt(&mgr, s_http_port, ev_handler, bind_opts);
  if (nc == NULL) {
    fprintf(stderr, "Error starting server on port %s: %s\n", s_http_port,
            *bind_opts.error_string);
    exit(1);
  }

  mg_set_protocol_http_websocket(nc);
  s_http_server_opts.enable_directory_listing = "yes";

  printf("Starting server on port %s\n", s_http_port);

  for (;;) {
    mg_mgr_poll(&mgr, 1000);
  }
  mg_mgr_free(&mgr);

  return 0;
}
