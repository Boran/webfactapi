<?php


/**
 * Updates a website
 * @param String $website_nid
 * @param Array $website_data
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
  $website_update_fields = array('hostname'=>'field_hostname', 'site_email'=>'field_site_email', 'public'=>'field_public', 'owner_uid'=>'field_site_owner_uid');

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
 * @param Array $website_data
 * @return Array result success, website node created nid
 *
 */
function _webfact_api_website_create($website_data) {


  //get the template nid
  $template_nid = $website_data['template_id'];
  if($template_nid == ''){
    $templates = node_load_multiple(array(), array('type' => 'template'));
    $template_nid = reset($templates)->nid;
  }

  //create website node
  $node = new stdClass();
  $node->type = 'website';
  $node->uid = 1;
  $node->title = $website_data['node_title'];
  $node->language = LANGUAGE_NONE;
  $node->field_hostname['und'][0]['value'] = $website_data['hostname'];
  $node->field_site_email['und'][0]['value'] = $website_data['site_email'];
  $node->field_public['und'][0]['value'] = $website_data['public'];
  $node->field_site_owner_uid['und'][0]['value'] = $website_data['owner_uid'];
  $node->field_template['und'][0]['target_id'] = $template_nid;

  // Passing values and adding final script:
  // MASTER_URL - site url to update.
  // OWENER_EMAIL - email address of user to be created.
  // PRODUCT_ID - id of product to update on master site.
  // DRUPAL_FINAL_SCRIPT - script to run.
  $node->field_docker_environment['und'][0]['value'] = 'MASTER_URL=' . $website_data['master_url'];
  $node->field_docker_environment['und'][1]['value'] = 'OWNER_MAIL=' . $website_data['owner_email'];
  $node->field_docker_environment['und'][2]['value'] = 'PRODUCT_ID=' . $website_data['product_id'];
  $node->field_docker_environment['und'][3]['value'] = 'DRUPAL_FINAL_SCRIPT=env_scripts/build_usr.sh';

  //save node
  node_save($node);

  //spin up the container
  $w = new WebfactController(1);
  if ($w) {
    $w->arguments('create',$node->nid,0);
  }

  //return result and nid of created website node
  return array('result' => 1, 'website_nid' => $node->nid);
}

/**
 * Retrieve all websites that fit $parameters
 * @param $parameters
 * Array associative keyed by field name , value is value
 */
function _webfact_api_website_retrieve_all($parameters) {

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
 */
function _webfact_api_website_retrieve($nid) {

  $node = node_load($nid);

  //get the website
  $website = _generate_website_resource($node);

  //return website object
  return $website;
}

/**
 * Helper function to generate website object ready for service return
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
  $w = new WebfactController(NULL, $node->nid);
  if ($w) {
    $website->status = $w->getStatus($node->nid);
    $website->build_status = $w->getContainerBuildStatus();
  }

  //Loop through fields to pull from website node
  $website_fields = array(
    'field_hostname' => 'hostname',
    'field_site_email' => 'site_email',
    'field_category' => 'category',
    'field_docker_restartpolicy' => 'restart policy',
    'field_template' => 'template',
    'field_public' => 'public',
    'field_site_owner_uid' => 'owner_uid',
  );

  foreach ($website_fields as $website_field => $website_field_title) {
    $field = field_get_items('node', $node, $website_field);
    //field_view_value not used as boolean not coming through
    $value = isset($field[0]["value"]) ? $field[0]["value"] : NULL;
    $website->{$website_field_title} = $value;
  }

  //website url
  $webfact_server = variable_get('webfact_fserver', 'mywildcard.example.ch');
  $website->url = $website->hostname.".".$webfact_server;

  return $website;
}