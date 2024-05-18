<?php

/**
 * @file
 * Theme setting callbacks for the garland theme.
 */

/**
 * Implements hook_form_FORM_ID_alter().
 *
 * @param $form
 *   The form.
 * @param $form_state
 *   The form state.
 */
function capito_form_system_theme_settings_alter(&$form, &$form_state) {
    $form['capito_font_awesome'] = array(
        '#type'          => 'radios',
        '#title'         => t('Use font awesome'),
        '#options'       => array(
            1 => t('Yes'),
            0 => t('No'),
        ),
        '#default_value' => theme_get_setting('capito_font_awesome'),
        '#description'   => t('Specify whether we use font awesome css library.'),
        '#weight'        => 0,
    );
    $form['capito_font_crete_round'] = array(
        '#type'          => 'radios',
        '#title'         => t('Use font crete round'),
        '#options'       => array(
            1 => t('Yes'),
            0 => t('No'),
        ),
        '#default_value' => theme_get_setting('capito_font_crete_round'),
        '#description'   => t('Specify whether we use font crete round.'),
        '#weight'        => 0,
    );
    $form['capito_bootstrap'] = array(
        '#type'          => 'radios',
        '#title'         => t('Use bootstrap 4.1.3'),
        '#options'       => array(
            1 => t('Yes'),
            0 => t('No'),
        ),
        '#default_value' => theme_get_setting('capito_bootstrap'),
        '#description'   => t('Specify whether we use bootstrap or not.'),
        '#weight'        => 0,
    );

}