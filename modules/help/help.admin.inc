<?php

/**
 * @file
 * Admin page callbacks for the help module.
 */

/**
 * Menu callback; prints a page listing a glossary of Drupal terminology.
 */
function help_main() {
  // Add CSS
  drupal_add_css(drupal_get_path('module', 'help') . '/help.css');
  $output = '<h2>' . t('Help topics') . '</h2><p>' . t('Help is available on the following items:') . '</p>' . help_links_as_list();
  return $output;
}

/**
 * Menu callback; prints a page listing general help for a module.
 *
 * @param $name
 *   A module name to display a help page for.
 */
function help_page($name) {
  $output = '';
  if (module_hook($name, 'help')) {
    $info = system_get_info('module');
    drupal_set_title($info[$name]['name']);

    $temp = module_invoke($name, 'help', "admin/help#$name", drupal_help_arg());
    if (empty($temp)) {
      $output .= t("No help is available for module %module.", array('%module' => $info[$name]['name']));
    }
    else {
      $output .= $temp;
    }

    // Only print list of administration pages if the module in question has
    // any such pages associated to it.
    $admin_tasks = system_get_module_admin_tasks($name, $info[$name]);
    if (!empty($admin_tasks)) {
      $links = array();
      foreach ($admin_tasks as $task) {
        $links[] = l($task['title'], $task['link_path'], $task['localized_options']);
      }
      $output .= theme('item_list', array('items' => $links, 'title' => t('@module administration pages', array('@module' => $info[$name]['name']))));
    }
  }
  return $output;
}

/**
 * Provides a formatted list of available help topics.
 *
 * @return
 *   A string containing the formatted list.
 */
function help_links_as_list() {
  $empty_arg = drupal_help_arg();
  $module_info = system_rebuild_module_data();

  $modules = array();
  foreach (module_implements('help', TRUE) as $module) {
    if (module_invoke($module, 'help', "admin/help#$module", $empty_arg)) {
      $modules[$module] = $module_info[$module]->info['name'];
    }
  }
  asort($modules);

  // Output pretty four-column list.
  $count = count($modules);
  $break = ceil($count / 4);
  $output = '<div class="clearfix"><div class="help-items"><ul>';
  $i = 0;
  foreach ($modules as $module => $name) {
    $output .= '<li>' . l($name, 'admin/help/' . $module) . '</li>';
    if (($i + 1) % $break == 0 && ($i + 1) != $count) {
      $output .= '</ul></div><div class="help-items' . ($i + 1 == $break * 3 ? ' help-items-last' : '') . '"><ul>';
    }
    $i++;
  }
  $output .= '</ul></div></div>';

  return $output;
}

