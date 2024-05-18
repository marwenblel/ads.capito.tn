<style>

  .car-ad .field-title {
    max-width: 350px;
    overflow: auto;
  }
</style>
<section class="regular slider">
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

<!-- we have comment this because file already called in top categories module
<script src="https://code.jquery.com/jquery-2.2.0.min.js" type="text/javascript"></script>
<script src="<?php //echo $variables['slick_cars'];?>/slick.js" type="text/javascript"></script>
-->
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
