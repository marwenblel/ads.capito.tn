<style>
div.user-teaser-block div.user-picture img {
        width: 100px;
        height: 100px;
        overflow: hidden;
        border-radius: 50%;
    }
div.user-teaser-block div.user-circle {
        background-color: #007bff;
        border-radius: 50%;
        text-align: center;
        line-height: 30px;
        font-weight: bold;
    }
</style>
<div class="user-teaser-block">
<center>
    <?php if (!empty($user_picture)): ?>
        <div class="user-picture"><?php print $user_picture; ?></div>
    <?php else: ?>
       <div class="user-picture">
           <a href="" title="View user profile."><img width="100" height="100" src="https://ads.capito.tn/sites/all/modules/custom/capito_user_teaser/images/default-avatar.png" alt="default picture" title="default picture"></a>
       </div>
    <?php endif; ?>
       <div class="user-link"><?php print l($username, 'user/' . $uid); ?> (<?php print $node_count.' annonces'; ?>)</div>
</center>
</div>
