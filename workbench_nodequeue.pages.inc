<?php


function workbench_nodequeue_overview() {
  $output = array();

  // Allow other modules to add content here.
  $output['#attributes'] = array('class' => array('admin', 'my-workbench'));
  $output['#attached'] = array(
    'css' => array(drupal_get_path('module', 'workbench') . '/css/workbench.my-workbench.css'),
  );
  $output['#theme'] = 'workbench_element';
  // This left column is given a width of 35% by workbench.myworkbench.css.
  $output['workbench_nodequeue_overview'] = array(
    '#title' => t('Node Queues Overview'),
    '#markup' => '<img src="http://www.thimothoeye.be/sub.png"/>',
    '#attributes' => array('class' => array('left', 'clearfix')),
    '#theme' => 'workbench_element',
  );
  
  return $output;
}


function workbench_nodequeue_queue_view($qid) {
  $module = 'nodequeue';
  $name = 'includes/nodequeue.admin';
  $type = 'inc';
  if (module_load_include($type, $module, $name) === FALSE) {
    return drupal_not_found();
  }
  $queue = nodequeue_load($qid);

  $subqueues = array();
  if ($queue->subqueues == 1) {
    $subqueues = nodequeue_load_subqueues_by_queue($queue->qid);
  } else {
    // @TODO Error reporting: Only simple queues allowed
    return drupal_not_found();
  }

  $subqueue = reset($subqueues);
  
  // // get nodes from the queue
  $nodes = _nodequeue_dragdrop_get_nodes($queue, $subqueue);
  return drupal_get_form('nodequeue_arrange_subqueue_form_' . $subqueue->sqid, $queue, $nodes, $subqueue);

}
