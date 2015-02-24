<?php


/**
 * Updates a website
 * @param $website_nid
 * @param $website_data
 *    Array website data : website_nid,hostname,site_email,node_data
 * @return Array result success
 *
 */
function _webfact_api_website_update($website_nid, $website_data) {

  //get the node
  $website_node = node_load($website_nid);

  //check node is valid
  if (!$website_node) {
    return array('result' => 0);
  }

  //update website node
  $website_update_fields = array('hostname'=>'field_hostname', 'site_email'=>'field_site_email', 'public'=>'field_public');

  //loop through possible updatable website fields
  foreach ($website_update_fields as $website_update_field_key =>$website_update_field_name) {
    //if api call sets data
    if (isset($website_data[$website_update_field_key])) {
      //set field names data
      $website_node->{$website_update_field_name}['und'][0]['value'] = $website_data[$website_update_field_key];
    }
  }
  if (isset($website_data['node_title'])) {
    $website_node->title = $website_data['node_title'];
  }

  //save node
  node_save($website_node);

  //return result
  return array('result' => 1);
}

/**
 * Creates a website
 * @param $website_data
 * Array website data : hostname,site_email,node_data
 * @return Array result success, website node created nid
 *
 */
function _webfact_api_website_create($website_data) {

  //get the default template
  $templates = node_load_multiple(array(), array('type' => 'template'));
  $template_nid = reset($templates)->nid;
  //create website node
  $node = new stdClass();
  $node->type = 'website';
  $node->uid = 1;
  $node->title = $website_data['node_title'];
  $node->language = LANGUAGE_NONE;
  $node->field_hostname['und'][0]['value'] = $website_data['hostname'];
  $node->field_site_email['und'][0]['value'] = $website_data['site_email'];
  $node->field_public['und'][0]['value'] = $website_data['public'];
  $node->field_template['und'][0]['target_id'] = $template_nid;
  //save node
  node_save($node);
  //return result and nid of created website node
  return array('result' => 1, 'website_nid' => $node->nid);

}

/**
 * Retrieve all websites that fit $parameters
 * @param $parameters
 * Array associative keyed by field name , value is value
 */
function _webfact_api_website_retrieve_all($parameters) {

  var_dump($parameters);

  //create base query
  $query = new EntityFieldQuery();
  $query->entityCondition('entity_type', 'node')
    ->entityCondition('bundle', 'website');

  //add conditions
  foreach ($parameters as $parameter => $parameter_value) {
    $query->fieldCondition($parameter, 'value', $parameter_value, '=');
  }

  //get result nodes
  $results = $query->execute();
  $node_ids = array_keys($results['node']);
  $nodes = node_load_multiple($node_ids);

  //create websites to return
  $websites = array();
  foreach ($nodes as $node) {
    $websites[] = _generate_website_resource($node);
  }

  return $websites;
}

/**
 * Returns a website
 *
 */
function _webfact_api_website_retrieve($nid) {

  $node = node_load($nid);

  //get the website
  $website = _generate_website_resource($node);

  //return website object
  return $website;
}

/**
 * Helper fucntion to generate website object ready for service return
 * @param $node
 * @return \stdClass
 */
function _generate_website_resource($node) {

  $website = new stdClass();

  $uri = entity_uri('node', $node);
  $node->path = url($uri['path'], array('absolute' => TRUE));
  // Unset uri as it has complete entity and this
  // cause never ending recursion in rendering.
  unset($node->uri);

  //node_id
  $website->node_id = $node->nid;

  // Website node title
  $website->node_title = $node->title;

  // Website status
  $w = new WebfactController;
  if ($w) {
    $website->status = $w->getStatus($node->nid);
  }

  //Loop through fields to pull from website node
  $website_fields = array(
    'field_hostname' => 'hostname',
    'field_site_email' => 'site_email',
    'field_category' => 'category',
    'field_docker_restartpolicy' => 'restart policy',
    'field_template' => 'template',
    'field_public' => 'public',
  );

  foreach ($website_fields as $website_field => $website_field_title) {
    $field = field_get_items('node', $node, $website_field);
    $output = field_view_value('node', $node, $website_field, $field[0]);
    if (isset($output['#markup'])) {
      $website->{$website_field_title} = $output['#markup'];
    }
  }

  return $website;


}