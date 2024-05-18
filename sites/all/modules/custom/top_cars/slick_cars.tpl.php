<style>
  .car-ad .field-title {
    max-width: 350px;
    overflow: auto;
  }
</style>
<div class="top-link">
    <a href="https://ads.capito.tn/search/vehicle-ad">Voir plus d'annonces&nbsp;&rarr;</a>
</div>
<section class="top-cars regular slider">
<?php foreach($variables['slick_cars_data'] as $key => $value) {?>
  <div class="car-ad">
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
		
		$(".top-cars.regular").slick({
        dots: true,
        infinite: true,
        slidesToShow: 5,
        slidesToScroll: 5
      });
    });
</script>
