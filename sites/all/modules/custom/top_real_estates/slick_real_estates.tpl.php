<style>
  .real-estate-ad .field-title {
    max-width: 350px;
    overflow: auto;
  }
</style>
<div class="top-link">
    <a href="">Voir plus d'annonces&nbsp;&rarr;</a>
</div>
<section class="top-real-estates regular slider">
<?php foreach($variables['slick_real_estates_data'] as $key => $value) {?>
  <div class="real-estate-ad">
    <div class="field field-image">
      <a href="<?php echo $value['url']; ?>">
        <img style="height: 200px; border-radius: 5px;" src="<?php echo $value['image']; ?>">
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
		$(".top-real-estates.regular").slick({
        dots: true,
        infinite: true,
        slidesToShow: 5,
        slidesToScroll: 5
      });
    });
</script>
