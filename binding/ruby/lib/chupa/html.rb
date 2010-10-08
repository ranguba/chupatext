require 'nokogiri'
class Chupa::HTML < Chupa
  def decompose
    decomposed(Nokogiri::HTML.parse(read).text)
  end
end
