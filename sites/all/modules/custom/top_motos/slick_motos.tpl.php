<style>
  .moto-ad .field-title {
    max-width: 350px;
    overflow: auto;
  }
</style>
<div class="top-link">
    <a href="">Voir plus d'annonces&nbsp;&rarr;</a>
</div>
<section class="regular slider">
<?php foreach($variables['slick_motos_data'] as $key => $value) {?>
  <div class="moto-ad">
    <div class="field field-image">
      <a href="<?php echo $value['link_node']; ?>">
        <img style="height: 300px;" src="<?php echo $value['image']; ?>">
      </a>
    </div>
    <div  class="field field-title"><?php echo $value['title']; ?></div>
    <div  class="field field-price"><?php echo $value['price']; echo '  DT'; ?></div>
    <div  class="field field-date"><?php echo $value['date']; ?></div>
  </div>
<?php }?>
</section>
<script type="text/javascript">
    $(document).on('ready', function() {
		
		$(".regular").slick({
        dots: true,
        infinite: true,
        slidesToShow: 3,
        slidesToScroll: 3
      });
    });
</script>
