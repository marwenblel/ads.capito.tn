<?php

/**
 * @file
 * Default theme implementation to display the basic html structure of a single
 * Drupal page.
 *
 * Variables:
 * - $css: An array of CSS files for the current page.
 * - $language: (object) The language the site is being displayed in.
 *   $language->language contains its textual representation.
 *   $language->dir contains the language direction. It will either be 'ltr' or 'rtl'.
 * - $rdf_namespaces: All the RDF namespace prefixes used in the HTML document.
 * - $grddl_profile: A GRDDL profile allowing agents to extract the RDF data.
 * - $head_title: A modified version of the page title, for use in the TITLE
 *   tag.
 * - $head_title_array: (array) An associative array containing the string parts
 *   that were used to generate the $head_title variable, already prepared to be
 *   output as TITLE tag. The key/value pairs may contain one or more of the
 *   following, depending on conditions:
 *   - title: The title of the current page, if any.
 *   - name: The name of the site.
 *   - slogan: The slogan of the site, if any, and if there is no title.
 * - $head: Markup for the HEAD section (including meta tags, keyword tags, and
 *   so on).
 * - $styles: Style tags necessary to import all CSS files for the page.
 * - $scripts: Script tags necessary to load the JavaScript files and settings
 *   for the page.
 * - $page_top: Initial markup from any modules that have altered the
 *   page. This variable should always be output first, before all other dynamic
 *   content.
 * - $page: The rendered page content.
 * - $page_bottom: Final closing markup from any modules that have altered the
 *   page. This variable should always be output last, after all other dynamic
 *   content.
 * - $classes String of classes that can be used to style contextually through
 *   CSS.
 *
 * @see template_preprocess()
 * @see template_preprocess_html()
 * @see template_process()
 *
 * @ingroup themeable
 */
?><!DOCTYPE html PUBLIC "-//W3C//DTD XHTML+RDFa 1.0//EN"
  "http://www.w3.org/MarkUp/DTD/xhtml-rdfa-1.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="<?php print $language->language; ?>" version="XHTML+RDFa 1.0" dir="<?php print $language->dir; ?>"<?php print $rdf_namespaces; ?>>

<head profile="<?php print $grddl_profile; ?>">
 <meta name="description" content="site de petites annonces en tunisie.">
 <meta name="keywords" content="voiture, immobilier, annonce, annonces, tunis, tunisie, occasion, vente, vendre, achat, acheter,">
 <meta name="viewport" content="initial-scale=1.0, width=device-width, maximum-scale=1.0, user-scalable=0"/>
  <?php print $head; ?>

  <title><?php print $head_title; ?></title>
  <?php print $styles; ?>
  <?php print $scripts; ?>
</head>
<body class="<?php print $classes; ?>" <?php print $attributes;?>>
<div id="site-menu-wrapper">
    <!-- toggled menu icon -->
    <a href="javascript:void(0);" id="toggled-icon" onclick="toggleMenu()">
        &#9776;
    </a>
    <!-- toggled menu icon -->
    <ul class="site-menu">
        <a href="https://ads.capito.tn"><li><img width="300px" height="80px" src="https://ads.capito.tn/sites/all/themes/custom/capito/logo.png" alt="Home"></li></a>
        <a href="https://ads.capito.tn"><li>Accueil</li></a>
        <a href="https://ads.capito.tn/deposer-une-annonce"><li>Déposer une annonce</li></a>
        <a href="https://ads.capito.tn/user/my-research"><li>Mes recherches</li></a>
        <a href="https://ads.capito.tn/user/my-favorite"><li>Favoris</li></a>
        <a href="https://ads.capito.tn/user/my-ads"><li>Mes annonces</li></a>
        <a href="https://ads.capito.tn"><li>Rechercher</li></a>
        <a href="https://ads.capito.tn"><li>Immobilier</li></a>
        <a href="https://ads.capito.tn/search/vehicle-ad"><li>Véhicules</li></a>
        <a href="https://ads.capito.tn"><li>Locations de vacances</li></a>
        <a href="https://ads.capito.tn"><li>Emploi</li></a>
        <a href="https://ads.capito.tn"><li>Mode</li></a>
        <a href="https://ads.capito.tn"><li>Maison & Jardin</li></a>
        <a href="https://ads.capito.tn"><li>Famille</li></a>
        <a href="https://ads.capito.tn"><li>Électronique</li></a>
        <a href="https://ads.capito.tn"><li>Loisirs</li></a>
        <a href="https://ads.capito.tn"><li>Autres</li></a>
		<?php if (user_is_logged_in()): ?>
        <a href="<?php print url('user/logout'); ?>"><li><strong>Me Déconnecter</strong></li></a>
        <?php else: ?>
        <a href="<?php print url('user/login'); ?>"><li><strong>Se connecter</strong></li></a>
        <?php endif; ?>
    </ul>
</div>
<div id="loader"></div>
<div id="content-loader" style="display:none;">
  <div id="skip-link">
    <a href="#main-content" class="element-invisible element-focusable"><?php print t('Skip to main content'); ?></a>
  </div>
  <?php print $page_top; ?>
  <?php print $page; ?>
  <?php print $page_bottom; ?>
</div>
</body>
</html>
