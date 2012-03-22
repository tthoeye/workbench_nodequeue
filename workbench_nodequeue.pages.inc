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


function workbench_nodequeue_bla() {
  dsm('bla');
  return array();
}