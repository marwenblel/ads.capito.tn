<?php
/**
 * @file
 * Contains the administration pages for Back To Top.
 *
 */

function remember_ltrv_settings($form, &$form_state) {

    $vocabulary = taxonomy_get_vocabularies();
    $checklist_vocab_array = array(); /* Change to array('0' => '--none--'); if you want a none option*/
    foreach ($vocabulary as $item) {
        $key = $item->machine_name;
        $value = $item->name;
        $checklist_vocab_array[$key] = $value;
    }

    $form['remember_ltrv_vocabulary'] = array(
        '#title' => t('Last selection vocabulary'),
        '#description' => t('Where should the last selection appear?'),
        '#type' => 'select',
        '#options' => $checklist_vocab_array,
        '#default_value' => variable_get('remember_ltrv_vocabulary', 1),
    );

    return system_settings_form($form);
}
