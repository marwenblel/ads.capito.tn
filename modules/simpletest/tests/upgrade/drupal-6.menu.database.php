<?php
db_insert('variable')->fields(array(
  'name',
  'value',
))
->values(array(
  'name' => 'menu_default_node_menu',
  'value' => 's:15:"secondary-links";',
))
->values(array(
  'name' => 'menu_primary_links_source',
  'value' => 's:15:"secondary-links";',
))
->values(array(
  'name' => 'menu_secondary_links_source',
  'value' => 's:13:"primary-links";',
))
->execute();

// Add some links to the menus.
db_insert('menu_links')->fields(array(
  'menu_name',
  'mlid',
  'plid',
  'link_path',
  'router_path',
  'link_title',
  'options',
  'module',
  'hidden',
  'external',
  'has_children',
  'expanded',
  'weight',
  'depth',
  'customized',
  'p1',
  'p2',
  'p3',
  'p4',
  'p5',
  'p6',
  'p7',
  'p8',
  'p9',
  'updated',
))
->values(array(
  'menu_name' => 'navigation',
  'mlid' => '201',
  'plid' => '0',
  'link_path' => 'node/add',
  'router_path' => 'node/add',
  'link_title' => 'nodeadd-navigation',
  'options' => 'a:0:{}',
  'module' => 'menu',
  'hidden' => '0',
  'external' => '0',
  'has_children' => '1',
  'expanded' => '0',
  'weight' => '1',
  'depth' => '1',
  'customized' => '0',
  'p1' => '201',
  'p2' => '0',
  'p3' => '0',
  'p4' => '0',
  'p5' => '0',
  'p6' => '0',
  'p7' => '0',
  'p8' => '0',
  'p9' => '0',
  'updated' => '0',
))
->values(array(
  'menu_name' => 'primary-links',
  'mlid' => '204',
  'plid' => '0',
  'link_path' => 'node/add',
  'router_path' => 'node/add',
  'link_title' => 'nodeadd-primary',
  'options' => 'a:0:{}',
  'module' => 'menu',
  'hidden' => '0',
  'external' => '0',
  'has_children' => '1',
  'expanded' => '0',
  'weight' => '1',
  'depth' => '1',
  'customized' => '0',
  'p1' => '204',
  'p2' => '0',
  'p3' => '0',
  'p4' => '0',
  'p5' => '0',
  'p6' => '0',
  'p7' => '0',
  'p8' => '0',
  'p9' => '0',
  'updated' => '0',
))
->values(array(
  'menu_name' => 'secondary-links',
  'mlid' => '205',
  'plid' => '0',
  'link_path' => 'node/add',
  'router_path' => 'node/add',
  'link_title' => 'nodeadd-secondary',
  'options' => 'a:0:{}',
  'module' => 'menu',
  'hidden' => '0',
  'external' => '0',
  'has_children' => '1',
  'expanded' => '0',
  'weight' => '1',
  'depth' => '1',
  'customized' => '0',
  'p1' => '205',
  'p2' => '0',
  'p3' => '0',
  'p4' => '0',
  'p5' => '0',
  'p6' => '0',
  'p7' => '0',
  'p8' => '0',
  'p9' => '0',
  'updated' => '0',
))
->values(array(
  'menu_name' => 'secondary-links',
  'mlid' => '206',
  'plid' => '0',
  'link_path' => 'node',
  'router_path' => 'node',
  'link_title' => 'node-page-with-query',
  'options' => 'a:2:{s:5:"query";s:14:"page=1&node=10";s:10:"attributes";a:1:{s:5:"title";s:0:"";}}',
  'module' => 'menu',
  'hidden' => '0',
  'external' => '0',
  'has_children' => '0',
  'expanded' => '0',
  'weight' => '2',
  'depth' => '1',
  'customized' => '1',
  'p1' => '206',
  'p2' => '0',
  'p3' => '0',
  'p4' => '0',
  'p5' => '0',
  'p6' => '0',
  'p7' => '0',
  'p8' => '0',
  'p9' => '0',
  'updated' => '0',
))
->execute();
db_insert('blocks')->fields(array(
  'bid',
  'module',
  'delta',
  'theme',
  'status',
  'weight',
  'region',
  'custom',
  'throttle',
  'visibility',
  'pages',
  'title',
  'cache',
))
->values(array(
  'bid' => '4',
  'module' => 'menu',
  'delta' => 'primary-links',
  'theme' => 'garland',
  'status' => '1',
  'weight' => '0',
  'region' => 'left',
  'custom' => '0',
  'throttle' => '0',
  'visibility' => '0',
  'pages' => '',
  'title' => 'My Primary Links',
  'cache' => '-1',
))
->values(array(
  'bid' => '5',
  'module' => 'menu',
  'delta' => 'secondary-links',
  'theme' => 'garland',
  'status' => '1',
  'weight' => '0',
  'region' => 'left',
  'custom' => '0',
  'throttle' => '0',
  'visibility' => '0',
  'pages' => '',
  'title' => 'My Secondary Links',
  'cache' => '-1',
))
->execute();
