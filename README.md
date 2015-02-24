Exposes an api to interact with website nodes

Retrieve specific website :

$options = array(
    'method' => 'GET',
    'timeout' => 15,
    'headers' => array('Content-Type' => 'application/json'),
);

$result = drupal_http_request('http://website-url/webfact_api/website/*nid*', $options);

***********************************************************

Retrieve all websites :

$data = array('field_public'=>1);

$retrieve_data = json_encode($data);

$options = array(
    'method' => 'GET',
    'data' => $retrieve_data,
    'timeout' => 15,
    'headers' => array('Content-Type' => 'application/json'),
);

$result = drupal_http_request('http://website/webfact_api/website*', $options);

*optional filter by field eg ?parameters[field_public]=0

***********************************************************

Update website :

$data = array('node_title'=>'xxxxx node title','hostname'=>'yyyyyy','site_email'=>'gggggg@test.com');

$update_data = json_encode($data);

$options = array(
    'method' => 'PUT',
    'data' => $update_data,
    'timeout' => 15,
    'headers' => array('Content-Type' => 'application/json'),
);

$result = drupal_http_request('http://website/webfact_api/website/*nid*', $options);

***********************************************************

Create website :

$data = array('node_title'=>'xxxxx node title','hostname'=>'yyyyyy','site_email'=>'gggggg@test.com');

$create_data = json_encode($data);

$options = array(
    'method' => 'POST',
    'data' => $create_data,
    'timeout' => 15,
    'headers' => array('Content-Type' => 'application/json'),
);

$result = drupal_http_request('http://website/webfact_api/website', $options);

***********************************************************
