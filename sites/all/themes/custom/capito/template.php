<?php
/**
 * Implements hook_process_html().
 * Override or insert variables into the page template.
 * TODO1: alter head title on front page.
 */
function capito_process_html(&$variables) {
    //TODO1
    $site_name   = $GLOBALS["conf"]["site_name"];
    $site_slogan = $GLOBALS["conf"]["site_slogan"];

    if($variables['is_front']) {
        $variables['head_title'] = $site_name. ' | '.$site_slogan;
    }
}
/**
 * Implements hook_preprocess_html().
 * DO2: add external awesome css library when theme_get_settings('font_awesome') = 1.
 */
function capito_preprocess_html(&$variables) {
    global $user;

    //DO1
    if(theme_get_setting('capito_font_awesome') == 1) {
        drupal_add_css('https://cdnjs.cloudflare.com/ajax/libs/font-awesome/4.7.0/css/font-awesome.min.css', array('type' => 'external'));
    }
    //DO2
    if(theme_get_setting('capito_font_crete_round') == 1) {
        drupal_add_css('https://fonts.googleapis.com/css?family=Crete Round', array('type' => 'external'));
    }
    //DO3
    if(theme_get_setting('capito_bootstrap') == 1) {
        drupal_add_css('https://stackpath.bootstrapcdn.com/bootstrap/4.1.3/css/bootstrap.min.css', array('type' => 'external'));

        drupal_add_js('https://code.jquery.com/jquery-3.3.1.slim.min.js', array('type' => 'external'));
        drupal_add_js('https://cdnjs.cloudflare.com/ajax/libs/popper.js/1.14.3/umd/popper.min.js', array('type' => 'external'));
        drupal_add_js('https://stackpath.bootstrapcdn.com/bootstrap/4.1.3/js/bootstrap.min.js', array('type' => 'external'));
    }
    // DO4: add class "user role" to body tag
    if (in_array('particular', $user->roles)) {
        // Add the CSS class to the body.
        $variables['classes_array'][] = 'particular';
    }
    elseif (in_array('professional', $user->roles)) {
        // Add the CSS class to the body.
        $variables['classes_array'][] = 'professional';
    }

}
/**
 * Implements hook_preprocess_page().
 * @param $vars
 */
function capito_preprocess_page(&$vars) {
    // - page--example.tpl.php
    if (isset($vars['node'])) {
        array_push($vars['theme_hook_suggestions'], 'page__'.$vars['node']->type);
        if (isset($vars['theme_hook_suggestions']) AND in_array('page__node__edit', $vars['theme_hook_suggestions'])) {
            array_push($vars['theme_hook_suggestions'], 'page__'.$vars['node']->type.'__edit');
        }
    }
}