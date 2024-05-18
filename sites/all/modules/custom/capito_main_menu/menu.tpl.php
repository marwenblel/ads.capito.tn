<nav class="main-menu">
    <?php
     if(count($variables['tree']) != 0) {
         foreach($variables['tree'] as $key => $item_lv1) {
             echo '<div class="btn-group">';
             echo '<button type="button" class="dropdown-toggle" data-toggle="dropdown" aria-haspopup="true" aria-expanded="false">';
             echo $item_lv1['link']['link_title'];
             echo '</button>';
             if($item_lv1['link']['has_children'] == 1) {
                 echo '<div class="dropdown-menu dropdown-menu-right" style="padding: 0px;">';
                 echo '<table  class="has-children">';
                 echo '<tr>';
                 foreach($item_lv1['below'] as $key1 => $item_lv2) {
                        $class = "no-children";
                     if($item_lv2['link']['has_children'] == 1) {
                         $class = "has-children";
                         echo '<td class="'.$class.'">';
                         echo '<button onclick="window.location.href = \''.$item_lv2['link']['link_path'].'\';" class="dropdown-header" type="button">'.$item_lv2['link']['link_title'].'</button>';
                         if($item_lv2['link']['has_children'] == 1) {
                             foreach($item_lv2['below'] as $key3 => $item_lv3) {
                                 echo '<a class="dropdown-item" href="'.$item_lv3['link']['link_path'].'">'.$item_lv3['link']['link_title'].'</a>';
                             }
                         }
                         echo '</td>';

                     }
                 }
                 echo '</tr>';
                 echo '</table>';

                 /* table with no children*/
                 echo '<table class="no-children">';
                 echo '<tr>';
                 foreach($item_lv1['below'] as $key1 => $item_lv2) {
                     $class = "no-children";
                     if($item_lv2['link']['has_children'] == 0) {
                         $class = "no-children";
                         echo '<td class="'.$class.'">';
                         echo '<button onclick="window.location.href = \''.$item_lv2['link']['link_path'].'\';" class="dropdown-header" type="button">'.$item_lv2['link']['link_title'].'</button>';
                         echo '</td>';
                     }
                 }
                 echo '</tr>';
                 echo '</table>';
                 /**/
                 echo '</div>';
             }
             echo '</div>';
         }
     }
    ?>
</nav>