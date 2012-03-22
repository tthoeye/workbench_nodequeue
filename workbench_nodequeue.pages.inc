<?php


function workbench_nodequeue_overview() {
  $module = 'nodequeue';
  $name = 'includes/nodequeue.admin';
  $type = 'inc';
  if (module_load_include($type, $module, $name) === FALSE) {
    return drupal_not_found();
  }
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
  
  // !! What follows is a shameless steal of nodequeue_view_queues !!
  // Fetch all of the queues.
  $queues = nodequeue_load_queues(nodequeue_get_all_qids(25));

  // Access Control
  foreach ($queues as $queue) {
    if (!nodequeue_queue_access($queue)) {
      unset($queues[$queue->qid]);
    }
  }

  if (empty($queues)) {
    return $output . t('No nodequeues exist.');
  }

  $header = array(
    array('data' => t('Title'), 'field' => 'title', 'sort' => 'asc'),
    array('data' => t('Max nodes'), 'field' => 'size'),
    array('data' => t('Current Nodes'), 'field' => 'subqueues'),
  );
  $table_sort = tablesort_init($header);

  $qids = array();
  $sort_primary = array();
  $sort_secondary = array();
  $sort_direction_regular = array('asc' => SORT_ASC, 'desc' => SORT_DESC);
  $sort_direction_reverse = array('asc' => SORT_DESC, 'desc' => SORT_ASC);
  foreach ($queues as $queue) {
    // If a queue has only one subqueue, store the qid so we can display
    // the number of nodes in the subqueue.
    if ($queue->subqueues == 1) {
      $qids[] = $queue->qid;
    }
    $sort_secondary[] = drupal_strtolower($queue->title);
    switch ($table_sort['sql']) {
      case 'title':
      default:
        $sort_primary[] = drupal_strtolower($queue->title);
        $sort_direction = $sort_direction_regular;
        break;
      case 'size':
        $sort_primary[] = $queue->size;
        $sort_direction = $sort_direction_reverse;
        break;
      case 'subqueues':
        $sort_primary[] = $queue->subqueues;
        $sort_direction = $sort_direction_regular;
        break;
    }
  }

  $subqueues = nodequeue_load_subqueues_by_queue($qids);
  // Relate all the subqueues we loaded back to our queues.
  foreach ($subqueues as $subqueue) {
    if (nodequeue_api_subqueue_access($subqueue, NULL, $queues[$subqueue->qid])) {
      $queues[$subqueue->qid]->subqueue = $subqueue;
    }
  }

  if (!empty($table_sort)) {
    if (strtolower($table_sort['sort']) == 'desc') {
      array_multisort($sort_primary, $sort_direction['desc'], $sort_secondary, $queues); // Re-indexes array keys; key no longer equals qid.
    }
    else {
      array_multisort($sort_primary, $sort_direction['asc'], $sort_secondary, $queues); // Re-indexes array keys; key no longer equals qid.
    }
  }

  $rows = array();
  foreach ($queues as $queue) {

    $sub_text = $queue->subqueues;

    // If this queue has only one subqueue.
    if ($sub_text == 1) {
      $sub_text = nodequeue_subqueue_size_text($queue->size, $queue->subqueue->count);
    }

    $link =  l(check_plain($queue->title), "admin/workbench/queues/$queue->qid");
    $rows[] = array(      
      array('data' => $link),
      array('data' => $queue->size == 0 ? t('Infinite') : $queue->size),
      array('data' => $sub_text),

    );
  }
  
  $output['workbench_nodequeue_list'] = array(
    '#attributes' => array('class' => array('right', 'clearfix')),
    '#theme' => 'workbench_element',
    '#markup' => theme('table', array('header' => $header, 'rows' => $rows))
  );
  
  // $output .= theme('pager', array('tags' => NULL));

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
